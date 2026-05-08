#include "DummyDataGenerator.h"
#include "SimpleJsonParser.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include <climits>

namespace fs = std::filesystem;

DummyDataGenerator::DummyDataGenerator(unsigned int seed)
    : m_orderCounter(0) {
    m_rng = seed ? std::mt19937(seed) : std::mt19937(std::random_device{}());

    m_template.sampleTypes        = {"Wafer", "Die", "Package", "Chip", "Module", "Substrate"};
    m_template.processSteps       = {"Lithography", "Etching", "CVD Deposition", "PVD Sputtering",
                                     "CMP", "Ion Implantation", "RTP Anneal", "Metallization",
                                     "Passivation", "Inspection", "Cleaning"};
    m_template.productionLines    = {"Line-A", "Line-B", "Line-C", "Line-D", "Line-E"};
    m_template.equipmentIds       = {"EQP-LIT-001", "EQP-LIT-002", "EQP-ETH-001", "EQP-ETH-002",
                                     "EQP-CVD-001", "EQP-PVD-001", "EQP-CMP-001", "EQP-IMP-001",
                                     "EQP-RTP-001", "EQP-MET-001", "EQP-INS-001", "EQP-CLN-001"};
    m_template.orderStatuses      = {"Pending", "Scheduled", "In Progress", "Quality Check",
                                     "Rework", "Completed", "On Hold", "Cancelled"};
    m_template.priorities         = {"Low", "Normal", "High", "Urgent", "Critical"};
    m_template.customers          = {"Samsung Electronics", "SK Hynix", "Micron Technology",
                                     "Intel Corporation", "TSMC", "GlobalFoundries", "UMC", "SMIC"};
    m_template.materialCodes      = {"MAT-SI-P100", "MAT-SI-N100", "MAT-GE-001", "MAT-SOI-001",
                                     "MAT-GAAS-001", "MAT-SIC-001", "MAT-GAN-001", "MAT-INP-001"};
    m_template.sampleNamePrefixes = {"Alpha", "Beta", "Gamma", "Delta", "Epsilon", "Zeta"};
    m_template.waferSizes         = {"150", "200", "300", "450"};
    m_template.nodeSizes          = {"7", "10", "14", "22", "28", "45", "65", "90", "130", "180"};
}

bool DummyDataGenerator::loadTemplate(const std::string& templatePath) {
    try {
        auto parser = SimpleJsonParser::fromFile(templatePath);

        auto tryArr = [&](const std::string& p, std::vector<std::string>& t) {
            auto a = parser.getStringArray(p);
            if (!a.empty()) t = a;
        };
        auto tryInt = [&](const std::string& p, int& t) {
            int v = parser.getInt(p, INT_MIN);
            if (v != INT_MIN) t = v;
        };
        auto tryDbl = [&](const std::string& p, double& t) {
            double v = parser.getNumber(p, -1e30);
            if (v != -1e30) t = v;
        };

        tryArr("templates.sample_types",        m_template.sampleTypes);
        tryArr("templates.process_steps",        m_template.processSteps);
        tryArr("templates.production_lines",     m_template.productionLines);
        tryArr("templates.equipment_ids",        m_template.equipmentIds);
        tryArr("templates.order_statuses",       m_template.orderStatuses);
        tryArr("templates.priorities",           m_template.priorities);
        tryArr("templates.customers",            m_template.customers);
        tryArr("templates.material_codes",       m_template.materialCodes);
        tryArr("templates.sample_name_prefixes", m_template.sampleNamePrefixes);
        tryArr("templates.wafer_sizes_mm",       m_template.waferSizes);
        tryArr("templates.node_sizes_nm",        m_template.nodeSizes);

        tryInt("ranges.quantity_min",             m_template.quantityMin);
        tryInt("ranges.quantity_max",             m_template.quantityMax);
        tryDbl("ranges.unit_price_min",           m_template.unitPriceMin);
        tryDbl("ranges.unit_price_max",           m_template.unitPriceMax);
        tryDbl("ranges.thickness_min_um",         m_template.thicknessMin);
        tryDbl("ranges.thickness_max_um",         m_template.thicknessMax);
        tryInt("ranges.defect_count_min",         m_template.defectCountMin);
        tryInt("ranges.defect_count_max",         m_template.defectCountMax);
        tryDbl("ranges.yield_rate_min",           m_template.yieldRateMin);
        tryDbl("ranges.yield_rate_max",           m_template.yieldRateMax);
        tryInt("ranges.due_date_offset_min_days", m_template.dueDateOffsetMin);
        tryInt("ranges.due_date_offset_max_days", m_template.dueDateOffsetMax);

        std::cout << "[OK] Template loaded: " << templatePath << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[WARN] Template load failed (" << e.what() << "), using defaults\n";
        return false;
    }
}

std::string DummyDataGenerator::randomFrom(const std::vector<std::string>& vec) {
    if (vec.empty()) return "";
    return vec[std::uniform_int_distribution<size_t>(0, vec.size() - 1)(m_rng)];
}

int DummyDataGenerator::randomInt(int min, int max) {
    if (min > max) std::swap(min, max);
    return std::uniform_int_distribution<int>(min, max)(m_rng);
}

double DummyDataGenerator::randomDouble(double min, double max) {
    if (min > max) std::swap(min, max);
    return std::uniform_real_distribution<double>(min, max)(m_rng);
}

std::string DummyDataGenerator::generateDate(int offsetDays) const {
    std::time_t t = std::time(nullptr) + static_cast<std::time_t>(offsetDays) * 86400;
    std::tm* tm_p = std::localtime(&t);
    std::ostringstream ss;
    ss << (tm_p->tm_year + 1900) << "-"
       << std::setfill('0') << std::setw(2) << (tm_p->tm_mon + 1) << "-"
       << std::setw(2) << tm_p->tm_mday;
    return ss.str();
}

std::string DummyDataGenerator::generateOrderId() {
    std::time_t t  = std::time(nullptr);
    std::tm*  tm_p = std::localtime(&t);
    std::ostringstream ss;
    ss << "ORD-"
       << (tm_p->tm_year + 1900)
       << std::setfill('0') << std::setw(2) << (tm_p->tm_mon + 1)
       << std::setw(2) << tm_p->tm_mday
       << "-" << std::setw(6) << (++m_orderCounter);
    return ss.str();
}

std::string DummyDataGenerator::generateLotNumber() {
    static const char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::ostringstream ss;
    ss << "LOT-" << randomInt(2020, 2026)
       << "-" << std::setfill('0') << std::setw(3) << randomInt(1, 999)
       << "-" << alpha[randomInt(0, 25)] << alpha[randomInt(0, 25)];
    return ss.str();
}

std::string DummyDataGenerator::generateOperatorId() {
    std::ostringstream ss;
    ss << "OPR-" << std::setfill('0') << std::setw(3) << randomInt(1, 200);
    return ss.str();
}

std::string DummyDataGenerator::generateSampleName(const std::string& prefix,
                                                    const std::string& type,
                                                    const std::string& node) {
    std::ostringstream ss;
    ss << prefix << "-" << type << "-" << node << "nm-"
       << static_cast<char>('A' + randomInt(0, 25))
       << std::setfill('0') << std::setw(3) << randomInt(1, 999);
    return ss.str();
}

ProductionOrder DummyDataGenerator::generateOrder() {
    ProductionOrder o;
    std::string nodeStr  = randomFrom(m_template.nodeSizes);
    std::string waferStr = randomFrom(m_template.waferSizes);
    std::string prefix   = randomFrom(m_template.sampleNamePrefixes);

    o.sampleType     = randomFrom(m_template.sampleTypes);
    o.nodeSizeNm     = nodeStr.empty()  ? 28  : std::stoi(nodeStr);
    o.waferSizeMm    = waferStr.empty() ? 300 : std::stoi(waferStr);
    o.orderId        = generateOrderId();
    o.customerId     = randomFrom(m_template.customers);
    o.sampleName     = generateSampleName(prefix, o.sampleType, nodeStr);
    o.lotNumber      = generateLotNumber();
    o.processStep    = randomFrom(m_template.processSteps);
    o.productionLine = randomFrom(m_template.productionLines);
    o.equipmentId    = randomFrom(m_template.equipmentIds);
    o.operatorId     = generateOperatorId();
    o.materialCode   = randomFrom(m_template.materialCodes);
    o.status         = randomFrom(m_template.orderStatuses);
    o.priority       = randomFrom(m_template.priorities);
    o.quantity       = randomInt(m_template.quantityMin, m_template.quantityMax);
    o.unitPrice      = randomDouble(m_template.unitPriceMin, m_template.unitPriceMax);
    o.orderDate      = generateDate(0);
    o.dueDate        = generateDate(randomInt(m_template.dueDateOffsetMin, m_template.dueDateOffsetMax));
    o.thicknessUm    = randomDouble(m_template.thicknessMin, m_template.thicknessMax);
    o.defectCount    = randomInt(m_template.defectCountMin, m_template.defectCountMax);
    o.yieldRate      = randomDouble(m_template.yieldRateMin, m_template.yieldRateMax);
    return o;
}

std::vector<ProductionOrder> DummyDataGenerator::generate(int count) {
    std::vector<ProductionOrder> orders;
    orders.reserve(static_cast<size_t>(count));
    for (int i = 0; i < count; ++i)
        orders.push_back(generateOrder());
    return orders;
}

std::string DummyDataGenerator::escapeJson(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '"':  r += "\\\""; break;
            case '\\': r += "\\\\"; break;
            case '\n': r += "\\n";  break;
            case '\r': r += "\\r";  break;
            case '\t': r += "\\t";  break;
            default:   r += c;
        }
    }
    return r;
}

std::string DummyDataGenerator::dblToStr(double d, int precision) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << d;
    return ss.str();
}

std::string DummyDataGenerator::orderToJson(const ProductionOrder& o, const std::string& indent) {
    const std::string i2 = indent + "  ";
    std::ostringstream ss;
    ss << indent << "{\n"
       << i2 << "\"order_id\": \""        << escapeJson(o.orderId)        << "\",\n"
       << i2 << "\"customer_id\": \""     << escapeJson(o.customerId)     << "\",\n"
       << i2 << "\"sample_name\": \""     << escapeJson(o.sampleName)     << "\",\n"
       << i2 << "\"sample_type\": \""     << escapeJson(o.sampleType)     << "\",\n"
       << i2 << "\"lot_number\": \""      << escapeJson(o.lotNumber)      << "\",\n"
       << i2 << "\"process_step\": \""    << escapeJson(o.processStep)    << "\",\n"
       << i2 << "\"production_line\": \"" << escapeJson(o.productionLine) << "\",\n"
       << i2 << "\"equipment_id\": \""    << escapeJson(o.equipmentId)    << "\",\n"
       << i2 << "\"operator_id\": \""     << escapeJson(o.operatorId)     << "\",\n"
       << i2 << "\"material_code\": \""   << escapeJson(o.materialCode)   << "\",\n"
       << i2 << "\"wafer_size_mm\": "     << o.waferSizeMm               << ",\n"
       << i2 << "\"node_size_nm\": "      << o.nodeSizeNm                << ",\n"
       << i2 << "\"status\": \""          << escapeJson(o.status)         << "\",\n"
       << i2 << "\"priority\": \""        << escapeJson(o.priority)       << "\",\n"
       << i2 << "\"quantity\": "          << o.quantity                   << ",\n"
       << i2 << "\"unit_price\": "        << dblToStr(o.unitPrice, 2)    << ",\n"
       << i2 << "\"order_date\": \""      << escapeJson(o.orderDate)      << "\",\n"
       << i2 << "\"due_date\": \""        << escapeJson(o.dueDate)        << "\",\n"
       << i2 << "\"thickness_um\": "      << dblToStr(o.thicknessUm, 3)  << ",\n"
       << i2 << "\"defect_count\": "      << o.defectCount               << ",\n"
       << i2 << "\"yield_rate\": "        << dblToStr(o.yieldRate, 2)    << "\n"
       << indent << "}";
    return ss.str();
}

std::string DummyDataGenerator::toJson(const std::vector<ProductionOrder>& orders) const {
    std::time_t now  = std::time(nullptr);
    std::tm*  tm_p   = std::localtime(&now);
    std::ostringstream ts;
    ts << (tm_p->tm_year + 1900)
       << "-" << std::setfill('0') << std::setw(2) << (tm_p->tm_mon + 1)
       << "-" << std::setw(2) << tm_p->tm_mday
       << "T" << std::setw(2) << tm_p->tm_hour
       << ":" << std::setw(2) << tm_p->tm_min
       << ":" << std::setw(2) << tm_p->tm_sec;

    std::ostringstream ss;
    ss << "{\n"
       << "  \"metadata\": {\n"
       << "    \"generated_at\": \"" << ts.str() << "\",\n"
       << "    \"count\": "          << orders.size() << ",\n"
       << "    \"generator_version\": \"1.0.0\",\n"
       << "    \"domain\": \"semiconductor_production_order\"\n"
       << "  },\n"
       << "  \"orders\": [\n";

    for (size_t i = 0; i < orders.size(); ++i) {
        ss << orderToJson(orders[i], "    ");
        if (i + 1 < orders.size()) ss << ",";
        ss << "\n";
    }

    ss << "  ]\n}\n";
    return ss.str();
}

bool DummyDataGenerator::saveToFile(const std::string& filePath,
                                    const std::vector<ProductionOrder>& orders) const {
    try {
        fs::path p(filePath);
        if (p.has_parent_path())
            fs::create_directories(p.parent_path());

        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Cannot open output file: " << filePath << "\n";
            return false;
        }
        file << toJson(orders);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving file: " << e.what() << "\n";
        return false;
    }
}
