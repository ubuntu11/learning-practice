CREATE TABLE afc_group (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  vendor_id INTEGER,
  quotation_id INTEGER,
  name VARCHAR(255),
  service_type INTEGER
);
CREATE INDEX IFK_GrpQuotationId on afc_group(quotation_id);

CREATE TABLE afc_resource (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  quotation_id INTEGER NOT NULL,
  resource_id INTEGER NOT NULL,
  name VARCHAR(255),
  serviceType INTEGER NOT NULL,
  FOREIGN KEY (quotation_id) references afc_group(quotation_id)
);
CREATE INDEX IFK_ResQuotationId on afc_resource(quotation_id);

CREATE TABLE group_performance (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  quotation_id INTEGER NOT NULL,
  execution_rate INTEGER NOT NULL,
  timestamp INTEGER NOT NULL,
  FOREIGN KEY (quotation_id) references afc_group(quotation_id)
);
CREATE INDEX IFK_GrpPerformQuotationId on group_performance(quotation_id);

CREATE TABLE group_real_power(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  quotation_id INTEGER NOT NULL,
  totalWatt INTEGER NOT NULL,
  timestamp INTEGER NOT NULL
);

CREATE TABLE resource_measurement(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  resource_id INTEGER NOT NULL,
  phv_a INTEGER NOT NULL,
  phv_b INTEGER NOT NULL,
  phv_c INTEGER NOT NULL,
  current_a INTEGER NOT NULL,
  current_b INTEGER NOT NULL,
  current_c INTEGER NOT NULL,
  frequency INTEGER NOT NULL,
  total_watt INTEGER NOT NULL,
  total_var INTEGER NOT NULL,
  power_factor INTEGER NOT NULL,
  soc INTEGER NOT NULL,
  ess_state INTEGER NOT NULL,
  timestamp INTEGER NOT NULL,
  FOREIGN KEY (resource_id) references afc_resource(id)
);
CREATE INDEX [IFK_ResMeasureResourceId] on resource_measurement(resource_id);

UPDATE SQLITE_SEQUENCE SET seq = 100 WHERE name = 'afc_group';
UPDATE SQLITE_SEQUENCE SET seq = 100 WHERE name = 'afc_resource';
UPDATE SQLITE_SEQUENCE SET seq = 100 WHERE name = 'group_performance';
UPDATE SQLITE_SEQUENCE SET seq = 100 WHERE name = 'group_real_power';
UPDATE SQLITE_SEQUENCE SET seq = 100 WHERE name = 'resource_measurement';

-- 上法若某表格尚未有第一筆資料是無法生效
-- https://stackoverflow.com/questions/692856/set-start-value-for-autoincrement-in-sqlite
BEGIN TRANSACTION;

UPDATE sqlite_sequence SET seq = <n> WHERE name = '<table>';

INSERT INTO sqlite_sequence (name,seq) SELECT '<table>', <n> WHERE NOT EXISTS
           (SELECT changes() AS change FROM sqlite_sequence WHERE change <> 0);
COMMIT;
