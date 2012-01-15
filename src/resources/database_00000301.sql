-- Products table
DROP TABLE IF EXISTS products;
CREATE TABLE IF NOT EXISTS products ( id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(50) UNIQUE, unit VARCHAR(50), expire VARCHAR(20), notes TEXT );
-- Batch table
DROP TABLE IF EXISTS batch;
CREATE TABLE IF NOT EXISTS batch ( id INTEGER PRIMARY KEY AUTOINCREMENT, prod_id INTEGER, spec VARCHAR(50), price INTEGER, unit VARCHAR(10), start_qty INTEGER, used_qty INTEGER, regdate VARCHAR(20), expirydate VARCHAR(20), entrydate VARCHAR(20), notes TEXT, invoice VARCHAR(50) );
-- Distributor table
DROP TABLE IF EXISTS distributor;
CREATE TABLE IF NOT EXISTS distributor ( id INTEGER PRIMARY KEY AUTOINCREMENT, batch_id INTEGER, quantity INTEGER, distdate VARCHAR(20), entrydate VARCHAR(20), disttype INTEGER, disttype_a VARCHAR(100), disttype_b VARCHAR(100) );
-- MealDay table
DROP TABLE IF EXISTS meal_day;
CREATE TABLE IF NOT EXISTS meal_day ( id INTEGER PRIMARY KEY AUTOINCREMENT, mealdate VARCHAR(20) UNIQUE, avcosts DOUBLE );
-- MealDay triggers
-- Meal table
DROP TABLE IF EXISTS meal;
CREATE TABLE IF NOT EXISTS meal ( id INTEGER PRIMARY KEY AUTOINCREMENT, mealday INTEGER, mealkind INTEGER, name VARCHAR(31), scouts INTEGER, leaders INTEGER, others INTEGER, cost INTEGER, notes TEXT);
-- Settings table
DROP TABLE IF EXISTS settings;
CREATE TABLE IF NOT EXISTS settings ( key VARCHAR(32) PRIMARY KEY UNIQUE, value VARCHAR(64) );


-- Products triggers
DROP TRIGGER IF EXISTS products_trig_delete;
CREATE TRIGGER IF NOT EXISTS products_trig_delete AFTER DELETE ON products FOR EACH ROW BEGIN DELETE FROM batch WHERE prod_id=OLD.id; END;
-- Batch triggers
DROP TRIGGER IF EXISTS batch_trig_delete;
CREATE TRIGGER IF NOT EXISTS batch_trig_delete AFTER DELETE ON batch FOR EACH ROW BEGIN DELETE FROM distributor WHERE batch_id=OLD.id; END;
-- Distributor triggers
DROP TRIGGER IF EXISTS distributor_trig_update;
CREATE TRIGGER IF NOT EXISTS distributor_trig_update AFTER UPDATE OF quantity ON distributor FOR EACH ROW BEGIN UPDATE batch SET used_qty=(SELECT sum(distributor.quantity) FROM distributor WHERE distributor.batch_id=OLD.batch_id) WHERE id=OLD.batch_id; END;
DROP TRIGGER IF EXISTS distributor_trig_insert;
CREATE TRIGGER IF NOT EXISTS distributor_trig_insert AFTER INSERT ON distributor FOR EACH ROW BEGIN UPDATE batch SET used_qty=(SELECT sum(distributor.quantity) FROM distributor WHERE distributor.batch_id=NEW.id) WHERE id=NEW.batch_id; END;
DROP TRIGGER IF EXISTS distributor_trig_delete;
CREATE TRIGGER IF NOT EXISTS distributor_trig_delete AFTER DELETE ON distributor FOR EACH ROW BEGIN UPDATE batch SET used_qty=(SELECT sum(distributor.quantity) FROM distributor WHERE distributor.batch_id=OLD.id) WHERE id=OLD.batch_id; END;
-- Meal triggers
DROP TRIGGER IF EXISTS meal_trig_delete;
-- CREATE TRIGGER IF NOT EXISTS meal_trig_delete AFTER DELETE ON meal FOR EACH ROW BEGIN DELETE FROM distributor WHERE reason3=2 AND reason=OLD.id; END;
-- CREATE TRIGGER IF NOT EXISTS meal_trig_sync AFTER UPDATE ON meal FOR EACH ROW BEGIN DELETE FROM distributor WHERE reason3=2 AND reason=OLD.id; END;

-- Settings vales
-- INSERT INTO settings VALUES (1, 0);
-- INSERT INTO settings VALUES (2, "");
-- INSERT INTO settings VALUES (4, "");
-- INSERT INTO settings VALUES (8, "");
-- INSERT INTO settings VALUES (16, "");
-- INSERT INTO settings VALUES (32, 0);
-- INSERT INTO settings VALUES (64, 0);
-- INSERT INTO settings VALUES (128, "");
-- INSERT INTO settings VALUES (256, "");
-- INSERT INTO settings VALUES (512, "");