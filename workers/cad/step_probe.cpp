#include <BRepBndLib.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Bnd_Box.hxx>
#include <Interface_CheckIterator.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPConstruct_ExternRefs.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_WorkSession.hxx>

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
constexpr qint64 max_bytes = 8 * 1024 * 1024;
constexpr int max_nodes = 1024;
constexpr int max_depth = 32;

QString entry(const TDF_Label& label) {
    TCollection_AsciiString id;
    TDF_Tool::Entry(label, id);
    return QString::fromLatin1(id.ToCString());
}

double metres(double mm) {
    if (!std::isfinite(mm)) throw std::runtime_error("nonfinite_coordinate");
    return mm / 1000.0;
}

QJsonObject describe_solid(const TDF_Label& definition, const TopLoc_Location& world) {
    const auto shape = XCAFDoc_ShapeTool::GetShape(definition);
    if (shape.IsNull() || shape.ShapeType() != TopAbs_SOLID ||
        !BRepCheck_Analyzer(shape).IsValid()) {
        throw std::runtime_error("expected_valid_solid");
    }
    Bnd_Box bounds;
    BRepBndLib::AddOptimal(shape.Moved(world), bounds, false, false);
    if (bounds.IsVoid() || bounds.IsOpen()) throw std::runtime_error("unbounded_solid");
    const auto low = bounds.CornerMin();
    const auto high = bounds.CornerMax();
    const auto& tr = world.Transformation();
    QJsonArray rotation;
    for (int row = 1; row <= 3; ++row) {
        rotation.append(QJsonArray{tr.Value(row, 1), tr.Value(row, 2), tr.Value(row, 3)});
    }
    return {{"definition_id", entry(definition)}, {"rotation", rotation},
            {"translation_m", QJsonArray{metres(tr.Value(1, 4)), metres(tr.Value(2, 4)), metres(tr.Value(3, 4))}},
            {"min_m", QJsonArray{metres(low.X()), metres(low.Y()), metres(low.Z())}},
            {"max_m", QJsonArray{metres(high.X()), metres(high.Y()), metres(high.Z())}}};
}

struct AssemblyReport {
    QJsonArray leaves;
    int visited = 0;

    void visit(const TDF_Label& node, const TopLoc_Location& parent, QJsonArray path) {
        if (++visited > max_nodes || path.size() >= max_depth) throw std::runtime_error("assembly_limit");
        path.append(entry(node));
        TDF_Label definition = node;
        auto world = parent;
        if (XCAFDoc_ShapeTool::IsReference(node)) {
            if (!XCAFDoc_ShapeTool::GetReferredShape(node, definition))
                throw std::runtime_error("unresolved_occurrence");
            world = parent * XCAFDoc_ShapeTool::GetLocation(node);
        }
        if (XCAFDoc_ShapeTool::IsAssembly(definition)) {
            TDF_LabelSequence children;
            XCAFDoc_ShapeTool::GetComponents(definition, children);
            if (children.IsEmpty()) throw std::runtime_error("empty_assembly");
            for (const auto& child : children) visit(child, world, path);
        } else {
            auto leaf = describe_solid(definition, world);
            leaf.insert("occurrence_path", path);
            leaves.append(leaf);
        }
    }
};

// FileUnits aggregates the units it finds. Check every shape representation too,
// so an explicit unit on one part cannot mask an unspecified unit on another.
void check_shape_units(const Handle(StepData_StepModel)& model) {
    for (int i = 1; i <= model->NbEntities(); ++i) {
        const auto rep = Handle(StepShape_ShapeRepresentation)::DownCast(model->Value(i));
        if (rep.IsNull()) continue;
        const auto context = rep->ContextOfItems();
        const auto with_uncertainty =
            Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx)::DownCast(context);
        const auto simple =
            Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)::DownCast(context);
        Handle(StepRepr_GlobalUnitAssignedContext) assigned;
        if (!with_uncertainty.IsNull()) assigned = with_uncertainty->GlobalUnitAssignedContext();
        else if (!simple.IsNull()) assigned = simple->GlobalUnitAssignedContext();
        if (assigned.IsNull()) throw std::runtime_error("expected_millimetre_context");
        int length_count = 0;
        for (int u = 1; u <= assigned->NbUnits(); ++u) {
            const auto length = Handle(StepBasic_SiUnitAndLengthUnit)::DownCast(assigned->UnitsValue(u));
            if (length.IsNull()) continue;
            if (!length->HasPrefix() || length->Prefix() != StepBasic_spMilli ||
                length->Name() != StepBasic_sunMetre) throw std::runtime_error("expected_millimetre_context");
            ++length_count;
        }
        if (length_count != 1) throw std::runtime_error("expected_millimetre_context");
    }
}

QJsonObject inspect(const QString& path) {
    QFile file(path);
    if (!QFileInfo(file).isFile() || !file.open(QIODevice::ReadOnly))
        throw std::runtime_error("input_unavailable");
    const auto bytes = file.read(max_bytes + 1);
    if (file.error() != QFileDevice::NoError) throw std::runtime_error("input_read_error");
    if (bytes.size() > max_bytes) throw std::runtime_error("input_too_large");
    if (!bytes.trimmed().endsWith("END-ISO-10303-21;")) throw std::runtime_error("incomplete_step_envelope");
    std::istringstream stream(bytes.toStdString());
    STEPCAFControl_Reader reader;
    if (reader.ReadStream("coupon.step", stream) != IFSelect_RetDone)
        throw std::runtime_error("step_read_failed");
    auto& step = reader.ChangeReader();
    if (!step.WS()->ModelCheckList().IsEmpty(true)) throw std::runtime_error("invalid_step_model");
    check_shape_units(step.StepModel());
    STEPConstruct_ExternRefs references(step.WS());
    references.LoadExternRefs();
    if (references.NbExternRefs() != 0) throw std::runtime_error("external_references_unsupported");
    TColStd_SequenceOfAsciiString length, angle, solid_angle;
    step.FileUnits(length, angle, solid_angle);
    if (length.Length() != 1 || length.First() != "millimetre")
        throw std::runtime_error("expected_millimetre_context");
    Handle(TDocStd_Document) doc = new TDocStd_Document("BinXCAF");
    XCAFDoc_DocumentTool::SetLengthUnit(doc, 1.0, UnitsMethods_LengthUnit_Millimeter);
    if (!reader.Transfer(doc)) throw std::runtime_error("xde_transfer_failed");
    TDF_LabelSequence roots;
    XCAFDoc_DocumentTool::ShapeTool(doc->Main())->GetFreeShapes(roots);
    if (roots.IsEmpty()) throw std::runtime_error("empty_document");
    AssemblyReport report;
    for (const auto& root : roots) report.visit(root, TopLoc_Location(), {});
    return {{"source_sha256", QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex())},
            {"file_length_unit", "mm"}, {"root_count", roots.Length()}, {"occurrences", report.leaves},
            {"release_eligible", false}};
}
} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    if (app.arguments().size() != 2) {
        std::cerr << "usage: kerobot_step_probe FILE.step\n";
        return 2;
    }
    Message::DefaultMessenger()->RemovePrinters(STANDARD_TYPE(Message_PrinterOStream));
    Message::DefaultMessenger()->AddPrinter(new Message_PrinterOStream("cerr", false));
    try {
        const auto report = inspect(app.arguments().at(1));
        std::cout << QJsonDocument(report).toJson(QJsonDocument::Compact).constData() << '\n';
        return 0;
    } catch (const Standard_Failure& e) {
        std::cerr << "occt_failure: " << e.GetMessageString() << '\n';
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 1;
}
