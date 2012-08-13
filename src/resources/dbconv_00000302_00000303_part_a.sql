--
DROP TRIGGER IF EXISTS b_upd;
--
DROP TRIGGER IF EXISTS b_up_price;
CREATE TRIGGER b_up_price AFTER UPDATE OF price ON batch FOR EACH ROW BEGIN UPDATE meal_day SET avcosts=round( ( SELECT sum( CAST ( ( SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id AND m.mealday=meal_day.id ) AS DOUBLE )/( m.scouts+m.leaders+m.others ) ) FROM meal AS m WHERE m.mealday=meal_day.id ) ) WHERE id IN ( SELECT md_w.id FROM meal_day AS md_w, meal AS m_w, distributor AS d_w WHERE d_w.batch_id=NEW.id AND d_w.disttype=2 AND m_w.id=d_w.disttype_a AND md_w.id=m_w.mealday ); END;
--
UPDATE settings SET value=(SELECT value*100 FROM settings WHERE key="AvgCosts") WHERE key="AvgCosts";
--
UPDATE batch SET price=(SELECT price FROM batch AS b WHERE b.id=batch.id);
--
INSERT OR REPLACE INTO settings VALUES ('dbversion', 771);