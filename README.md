# DummyDataGenerator

**반도체 시료 생산주문관리** 테스트용 더미 데이터 생성 도구 (C++ PoC)

## 개요

반도체 시료 생산주문 관리 시스템 개발 및 테스트에 필요한 현실적인 더미 데이터를 JSON 형식으로 생성합니다.
JSON 템플릿 파일을 기반으로 다양한 시나리오의 생산주문 데이터를 빠르게 만들 수 있습니다.

## 빌드 요구사항

- C++17 이상 컴파일러 (GCC 9+, Clang 9+, MSVC 2019+)
- CMake 3.16 이상

## 빌드

```bash
git clone https://github.com/esspks21/DummyDataGenerator-KyusungPark-19017482.git
cd DummyDataGenerator-KyusungPark-19017482

cmake -B build -S .
cmake --build build --config Release
```

## 사용법

```bash
# 기본 실행 (10개 주문 생성)
./build/bin/DummyDataGenerator

# 100개 주문 생성
./build/bin/DummyDataGenerator -n 100

# 재현 가능한 데이터 생성 (시드 고정)
./build/bin/DummyDataGenerator -n 50 -s 42

# 출력 파일 지정
./build/bin/DummyDataGenerator -n 200 -o output/test_orders.json

# 커스텀 템플릿 사용
./build/bin/DummyDataGenerator -t my_template.json -n 50

# 도움말
./build/bin/DummyDataGenerator -h
```

## 옵션

| 옵션 | 기본값 | 설명 |
|------|--------|------|
| `-n <count>` | `10` | 생성할 주문 수 |
| `-t <path>` | `data/templates/production_order_template.json` | 템플릿 파일 경로 |
| `-o <path>` | `output/dummy_orders.json` | 출력 파일 경로 |
| `-s <seed>` | 랜덤 | 재현용 랜덤 시드 |
| `-h` | - | 도움말 |

## 출력 형식

```json
{
  "metadata": {
    "generated_at": "2026-05-08T10:30:00",
    "count": 10,
    "generator_version": "1.0.0",
    "domain": "semiconductor_production_order"
  },
  "orders": [
    {
      "order_id": "ORD-20260508-000001",
      "customer_id": "Samsung Electronics",
      "sample_name": "Alpha-Wafer-7nm-A042",
      "sample_type": "Wafer",
      "lot_number": "LOT-2024-157-KR",
      "process_step": "Lithography",
      "production_line": "Line-A",
      "equipment_id": "EQP-LIT-001",
      "operator_id": "OPR-042",
      "material_code": "MAT-SI-P100",
      "wafer_size_mm": 300,
      "node_size_nm": 7,
      "status": "In Progress",
      "priority": "High",
      "quantity": 25,
      "unit_price": 15000.00,
      "order_date": "2026-05-08",
      "due_date": "2026-07-15",
      "thickness_um": 0.850,
      "defect_count": 3,
      "yield_rate": 95.20
    }
  ]
}
```

## 생성 필드 설명

| 필드 | 설명 |
|------|------|
| `order_id` | 주문 고유 ID (`ORD-YYYYMMDD-NNNNNN`) |
| `customer_id` | 고객사 |
| `sample_name` | 시료명 (prefix + type + node + 일련번호 조합) |
| `sample_type` | 시료 종류 (Wafer, Die, Package, Chip, Module, Substrate) |
| `lot_number` | 로트 번호 (`LOT-YYYY-NNN-XX`) |
| `process_step` | 공정 단계 (Lithography, Etching 등 11종) |
| `production_line` | 생산 라인 (Line-A ~ Line-E) |
| `equipment_id` | 사용 장비 ID |
| `operator_id` | 작업자 ID (`OPR-NNN`) |
| `material_code` | 원자재 코드 (Si, GaAs, SiC, GaN 등) |
| `wafer_size_mm` | 웨이퍼 직경 mm (150 / 200 / 300 / 450) |
| `node_size_nm` | 공정 노드 nm (7 ~ 180) |
| `status` | 주문 상태 (Pending → Completed 등 8종) |
| `priority` | 우선순위 (Low / Normal / High / Urgent / Critical) |
| `quantity` | 수량 |
| `unit_price` | 단가 |
| `order_date` | 주문 날짜 |
| `due_date` | 납기 날짜 |
| `thickness_um` | 두께 μm |
| `defect_count` | 결함 수 |
| `yield_rate` | 수율 % |

## 템플릿 커스터마이징

`data/templates/production_order_template.json` 을 수정하여 생성 값의 범위와 목록을 변경할 수 있습니다.

```json
{
  "templates": {
    "customers": ["My Company A", "My Company B"],
    "process_steps": ["Step 1", "Step 2"]
  },
  "ranges": {
    "quantity_min": 10,
    "quantity_max": 1000
  }
}
```

## 라이선스

MIT License
