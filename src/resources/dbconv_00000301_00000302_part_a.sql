--
DROP TRIGGER IF EXISTS d_ins;
CREATE TRIGGER d_ins    AFTER INSERT ON distributor                   BEGIN UPDATE batch SET used_qty=(SELECT sum(d.quantity) FROM distributor AS d WHERE d.batch_id=batch.id) WHERE id=NEW.batch_id; UPDATE meal_day SET avcosts=round(( SELECT sum( CAST(( SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id ) AS DOUBLE)/( m.scouts+m.leaders+m.others )) FROM meal AS m WHERE m.mealday=meal_day.id )) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m WHERE (NEW.disttype=2 AND m.id=NEW.disttype_a) AND md.id=m.mealday); END;
DROP TRIGGER IF EXISTS d_del;
CREATE TRIGGER d_del    AFTER DELETE ON distributor                   BEGIN UPDATE batch SET used_qty=(SELECT sum(d.quantity) FROM distributor AS d WHERE d.batch_id=batch.id) WHERE id=OLD.batch_id; UPDATE meal_day SET avcosts=round(( SELECT sum( CAST(( SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id ) AS DOUBLE)/( m.scouts+m.leaders+m.others )) FROM meal AS m WHERE m.mealday=meal_day.id )) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m WHERE (OLD.disttype=2 AND m.id=OLD.disttype_a) AND md.id=m.mealday); END;
DROP TRIGGER IF EXISTS d_up_bid;
CREATE TRIGGER d_up_bid AFTER UPDATE OF batch_id ON distributor       FOR EACH ROW BEGIN UPDATE batch SET used_qty=(SELECT sum(d.quantity) FROM distributor AS d WHERE d.batch_id=batch.id) WHERE id=OLD.batch_id OR id=NEW.batch_id; END;
DROP TRIGGER IF EXISTS d_up_qty;
CREATE TRIGGER d_up_qty AFTER UPDATE OF quantity ON distributor       FOR EACH ROW BEGIN UPDATE meal_day SET avcosts=round(( SELECT sum( CAST(( SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id ) AS DOUBLE)/( m.scouts+m.leaders+m.others )) FROM meal AS m WHERE m.mealday=meal_day.id )) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m WHERE (NEW.disttype=2 AND m.id=NEW.disttype_a) AND md.id=m.mealday); END;
--
DROP TRIGGER IF EXISTS m_upd;
CREATE TRIGGER m_upd    AFTER UPDATE OF scouts,leaders,others ON meal FOR EACH ROW BEGIN UPDATE meal_day SET avcosts=round(( SELECT sum( CAST(( SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id ) AS DOUBLE)/( m.scouts+m.leaders+m.others )) FROM meal AS m WHERE m.mealday=meal_day.id )) WHERE id=NEW.mealday; END;
--
INSERT OR REPLACE INTO settings VALUES ('dbversion', 770);