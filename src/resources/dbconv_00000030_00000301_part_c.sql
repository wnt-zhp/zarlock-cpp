-- Update meal_day
-- UPDATE meal_day SET avcosts=(SELECT (sum(d.quantity*b.price)/(m.scouts+m.leaders+m.others)/10000.0) FROM meal AS m, distributor AS d, batch as b WHERE m.mealday=meal_day.id AND d.disttype_a=m.id AND b.id=d.batch_id);
-- Delete old tables
DROP TABLE IF EXISTS products_tmp;
DROP TABLE IF EXISTS batch_tmp;
DROP TABLE IF EXISTS distributor_tmp;
DROP TABLE IF EXISTS meal_tmp;
DROP TABLE IF EXISTS settings_tmp;
