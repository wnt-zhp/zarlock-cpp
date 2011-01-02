DROP TABLE IF EXISTS products;
CREATE TABLE IF NOT EXISTS products ( id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(50) UNIQUE, unit VARCHAR(50), expire VARCHAR(20) );
CREATE TRIGGER IF NOT EXISTS products_trig_delete AFTER DELETE ON products FOR EACH ROW BEGIN DELETE FROM batch WHERE prod_id=OLD.id; END;

DROP TABLE IF EXISTS batch;
CREATE TABLE IF NOT EXISTS batch ( id INTEGER PRIMARY KEY AUTOINCREMENT, prod_id INTEGER, spec VARCHAR(50), price VARCHAR(20), unit VARCHAR(50), start_qty FLOAT, booking VARCHAR(20), expire VARCHAR(20), curr_qty FLOAT, date VARCHAR(20), desc TEXT, invoice_no VARCHAR(200) );

INSERT INTO products VALUES (1,"chleb","1kg","+100");
INSERT INTO products VALUES (2,"miód","1l","+100");
INSERT INTO products VALUES (3,"dżem","250g","+200");

INSERT INTO batch VALUES (1,1,"jasny","2+7","1kg",20.0,"0","+10",20.0,"22/12/2010",":)","svdft2fdsfy");
INSERT INTO batch VALUES (2,1,"ciemny","2+7","1kg",20.0,"0","+10",20.0,"22/12/2010",":)","svdft2fdsfy");
INSERT INTO batch VALUES (3,2,"jasny","2+7","1kg",20.0,"0","+10",20.0,"22/12/2010",":)","svdft2fdsfy");
INSERT INTO batch VALUES (4,2,"ciemny","2+7","1kg",20.0,"0","+10",20.0,"22/12/2010",":)","svdft2fdsfy");

