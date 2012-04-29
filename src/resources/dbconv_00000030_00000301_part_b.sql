
-- Update products
INSERT INTO products SELECT * FROM products_tmp;
-- Update batch
INSERT INTO batch(id,prod_id,spec,price,unit,start_qty,used_qty,regdate,expirydate,entrydate,notes,invoice) SELECT id,prod_id,spec,price,unit,(start_qty*100),(used_qty*100),booking,expire,date,notes,invoice_no FROM batch_tmp;
-- Update distributor
INSERT INTO distributor SELECT id,batch_id,(quantity*100),distdate,registered,reason3,reason,reason2 FROM distributor_tmp;
-- Create meal_day
INSERT INTO meal_day SELECT DISTINCT (null),distdate,0 FROM distributor_tmp WHERE distdate!="" ORDER BY distdate ASC;
-- Updating meal
INSERT INTO meal SELECT DISTINCT null,CAST(md.id as INTEGER),CAST(d.reason AS INTEGER),CAST(d.reason AS INTEGER),m.scouts,m.leaders,m.others,0,d.distdate FROM meal_day AS md, distributor_tmp AS d, meal_tmp AS m WHERE d.reason3=2 AND d.distdate=md.mealdate AND m.distdate=md.mealdate ORDER BY d.distdate,d.reason;
UPDATE distributor SET disttype_a=(SELECT meal.id FROM meal,distributor_tmp,meal_day WHERE meal_day.mealdate=distributor_tmp.distdate AND meal.mealday=meal_day.id AND meal.mealkind=distributor_tmp.reason AND distributor_tmp.id=distributor.id);
-- Update settings
INSERT OR REPLACE INTO settings(key, value) SELECT key, value FROM settings_tmp;
UPDATE settings SET key="IsDatabaseCorrect" WHERE key="1";
UPDATE settings SET key="CampName" WHERE key="2";
UPDATE settings SET key="CampPlace" WHERE key="3";
UPDATE settings SET key="CampOrg" WHERE key="4";
UPDATE settings SET key="CampDateBegin" WHERE key="5";
UPDATE settings SET key="CampDateEnd" WHERE key="6";
UPDATE settings SET key="ScoutsNo" WHERE key="7";
UPDATE settings SET key="LeadersNo" WHERE key="8";
UPDATE settings SET key="AvgCosts" WHERE key="9";
UPDATE settings SET key="CampLeader" WHERE key="10";
UPDATE settings SET key="CampQuarter" WHERE key="11";
UPDATE settings SET key="CampOthers" WHERE key="12";
UPDATE settings SET value=769 WHERE key='dbversion';

-- Update batch
UPDATE batch SET used_qty=(SELECT sum(quantity) FROM distributor WHERE batch_id=batch.id);
UPDATE meal SET cost=(SELECT (d.quantity*b.price) FROM meal AS m, distributor AS d, batch as b WHERE m.id=meal.id AND d.disttype_a=m.id AND b.id=d.batch_id);
UPDATE meal SET name="Śniadanie" WHERE meal.mealkind=0;
UPDATE meal SET name="Drugie śniadanie" WHERE meal.mealkind=1;
UPDATE meal SET name="Obiad" WHERE meal.mealkind=2;
UPDATE meal SET name="Podwieczorek" WHERE meal.mealkind=3;
UPDATE meal SET name="Kolacja" WHERE meal.mealkind=4;
UPDATE meal SET name="Inny" WHERE meal.mealkind=5;