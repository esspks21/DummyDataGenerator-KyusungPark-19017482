#pragma once
#include <string>
#include <vector>
#include <random>

struct ProductionOrder {
    std::string orderId;
    std::string customerId;
    std::string sampleName;
    std::string sampleType;
    std::string lotNumber;
    std::string processStep;
    std::string productionLine;
    std::string equipmentId;
    std::string operatorId;
    std::string materialCode;
    int         waferSizeMm;
    int         nodeSizeNm;
    std::string status;
    std::string priority;
    int         quantity;
    double      unitPrice;
    std::string orderDate;
    std::string dueDate;
    double      thicknessUm;
    int         defectCount;
    double      yieldRate;
};

struct OrderTemplate {
    std::vector<std::string> sampleTypes;
    std::vector<std::string> processSteps;
    std::vector<std::string> productionLines;
    std::vector<std::string> equipmentIds;
    std::vector<std::string> orderStatuses;
    std::vector<std::string> priorities;
    std::vector<std::string> customers;
    std::vector<std::string> materialCodes;
    std::vector<std::string> sampleNamePrefixes;
    std::vector<std::string> waferSizes;
    std::vector<std::string> nodeSizes;

    int    quantityMin = 1,         quantityMax = 100;
    double unitPriceMin = 1000.0,   unitPriceMax = 50000.0;
    double thicknessMin = 0.1,      thicknessMax = 2.0;
    int    defectCountMin = 0,      defectCountMax = 50;
    double yieldRateMin = 60.0,     yieldRateMax = 99.9;
    int    dueDateOffsetMin = 7,    dueDateOffsetMax = 90;
};

class DummyDataGenerator {
public:
    explicit DummyDataGenerator(unsigned int seed = 0);

    bool loadTemplate(const std::string& templatePath);
    std::vector<ProductionOrder> generate(int count);
    bool saveToFile(const std::string& filePath, const std::vector<ProductionOrder>& orders) const;
    std::string toJson(const std::vector<ProductionOrder>& orders) const;

private:
    OrderTemplate m_template;
    std::mt19937  m_rng;
    int           m_orderCounter;

    std::string randomFrom(const std::vector<std::string>& vec);
    int         randomInt(int min, int max);
    double      randomDouble(double min, double max);

    ProductionOrder generateOrder();
    std::string     generateOrderId();
    std::string     generateLotNumber();
    std::string     generateOperatorId();
    std::string     generateDate(int offsetDays = 0) const;
    std::string     generateSampleName(const std::string& prefix,
                                       const std::string& type,
                                       const std::string& node);

    static std::string orderToJson(const ProductionOrder& order, const std::string& indent);
    static std::string escapeJson(const std::string& s);
    static std::string dblToStr(double d, int precision = 2);
};
