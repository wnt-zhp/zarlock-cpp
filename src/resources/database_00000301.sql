-- Products table
DROP TABLE IF EXISTS products;
CREATE TABLE IF NOT EXISTS products ( id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(50) UNIQUE, unit VARCHAR(50), expire VARCHAR(20), notes TEXT );
-- Batch table
DROP TABLE IF EXISTS batch;
CREATE TABLE IF NOT EXISTS batch ( id INTEGER PRIMARY KEY AUTOINCREMENT, prod_id INTEGER, spec VARCHAR(50), price INTEGER, unit VARCHAR(10), start_qty INTEGER, used_qty INTEGER, regdate VARCHAR(20), expirydate VARCHAR(20), entrydate VARCHAR(20), invoice VARCHAR(50), notes TEXT );
-- Distributor table
DROP TABLE IF EXISTS distributor;
CREATE TABLE IF NOT EXISTS distributor ( id INTEGER PRIMARY KEY AUTOINCREMENT, batch_id INTEGER, quantity INTEGER, distdate VARCHAR(20), entrydate VARCHAR(20), disttype INTEGER, disttype_a VARCHAR(100), disttype_b VARCHAR(100) DEFAULT NULL );
-- MealDay table
DROP TABLE IF EXISTS meal_day;
CREATE TABLE IF NOT EXISTS meal_day ( id INTEGER PRIMARY KEY AUTOINCREMENT, mealdate VARCHAR(20) UNIQUE, avcosts INTEGER );
-- MealDay triggers
-- Meal table
DROP TABLE IF EXISTS meal;
CREATE TABLE IF NOT EXISTS meal ( id INTEGER PRIMARY KEY AUTOINCREMENT, mealday INTEGER, mealkind INTEGER, name VARCHAR(31), scouts INTEGER, leaders INTEGER, others INTEGER, cost INTEGER, notes TEXT);
-- Settings table
DROP TABLE IF EXISTS settings;
CREATE TABLE IF NOT EXISTS settings ( key VARCHAR(32) PRIMARY KEY UNIQUE, value VARCHAR(64) );
-- Products triggers
CREATE TRIGGER IF NOT EXISTS p_delete AFTER DELETE ON products FOR EACH ROW BEGIN DELETE FROM batch WHERE prod_id=OLD.id; END;
-- Batch triggers
CREATE TRIGGER IF NOT EXISTS b_delete AFTER DELETE ON batch FOR EACH ROW BEGIN DELETE FROM distributor WHERE batch_id=OLD.id; END;
CREATE TRIGGER b_up_price AFTER UPDATE OF price ON batch FOR EACH ROW BEGIN UPDATE meal_day SET avcosts=round(CAST((SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d, meal AS m WHERE d.batch_id=b.id AND d.disttype=2 AND d.disttype_a=m.id AND m.mealday=meal_day.id) AS DOUBLE)/(SELECT sum(m.scouts+m.leaders+m.others) FROM meal as m WHERE mealday=meal_day.id)) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m, distributor AS d WHERE d.disttype=2 AND m.id=d.disttype_a AND d.batch_id=NEW.id AND md.id=m.mealday); END;
-- Distributor triggers
CREATE TRIGGER d_ins    AFTER INSERT ON distributor                      FOR EACH ROW BEGIN UPDATE batch SET used_qty=(SELECT sum(d.quantity) FROM distributor AS d WHERE d.batch_id=batch.id) WHERE id=NEW.batch_id; UPDATE meal_day SET avcosts=round(CAST((SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d, meal AS m WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id AND m.mealday=meal_day.id) AS DOUBLE)/(SELECT sum(m.scouts+m.leaders+m.others) FROM meal as m WHERE mealday=meal_day.id)) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m, distributor AS d WHERE (NEW.disttype=2 AND m.id=NEW.disttype_a) AND md.id=m.mealday); END;
CREATE TRIGGER d_del    AFTER DELETE ON distributor                      FOR EACH ROW BEGIN UPDATE batch SET used_qty=(SELECT sum(d.quantity) FROM distributor AS d WHERE d.batch_id=batch.id) WHERE id=OLD.batch_id; UPDATE meal_day SET avcosts=round(CAST((SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d, meal AS m WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id AND m.mealday=meal_day.id) AS DOUBLE)/(SELECT sum(m.scouts+m.leaders+m.others) FROM meal as m WHERE mealday=meal_day.id)) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m, distributor AS d WHERE (OLD.disttype=2 AND m.id=OLD.disttype_a) AND md.id=m.mealday); END;
CREATE TRIGGER d_up_qty AFTER UPDATE OF batch_id,quantity ON distributor FOR EACH ROW BEGIN UPDATE batch SET used_qty=(SELECT sum(d.quantity) FROM distributor AS d WHERE d.batch_id=batch.id) WHERE id=OLD.batch_id OR id=NEW.batch_id; UPDATE meal_day SET avcosts=round(CAST((SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d, meal AS m WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id AND m.mealday=meal_day.id) AS DOUBLE)/(SELECT sum(m.scouts+m.leaders+m.others) FROM meal as m WHERE mealday=meal_day.id)) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m, distributor AS d WHERE (NEW.disttype=2 AND m.id=NEW.disttype_a) AND md.id=m.mealday); END;

CREATE TRIGGER m_upd    AFTER UPDATE OF scouts,leaders,others ON meal FOR EACH ROW BEGIN UPDATE meal_day SET avcosts=round(CAST((SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=NEW.id) AS DOUBLE)/(SELECT sum(m.scouts+m.leaders+m.others) FROM meal as m WHERE id=NEW.id)) WHERE id=NEW.mealday; END;
-- DROP TRIGGER IF EXISTS d_del;
-- Meal triggers
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
-- INSERT INTO settings VALUES (51