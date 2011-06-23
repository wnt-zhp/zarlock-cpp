INSERT INTO products VALUES (1, "chleb", "1kg", "+100", ":)");
INSERT INTO products VALUES (2, "miód", "1l", "+100", ":)");
INSERT INTO products VALUES (3, "dżem", "250g", "+200", ":)");

INSERT INTO batch VALUES (1, 1, "jasny", "2+7", "1kg", 20.0, "2010-12-22", "+10", 0.0, "2010-12-22", ":)", "svdft2fdsfy");
INSERT INTO batch VALUES (2, 1, "ciemny", "2+7", "1kg", 20.0, "2010-12-22", "+10", 0.0, "2010-12-22", ":)", "svdft2fdsfy");
INSERT INTO batch VALUES (3, 1, "pełnoziarnisty", "2+7", "1kg", 20.0, "2010-12-22", "+10", 0.0, "2010-12-22", ":)", "svdft2fdsfy");
INSERT INTO batch VALUES (4, 2, "lipowy", "2+7", "1l", 20.0, "2010-12-22", "+10", 0.0, "2011-04-10", ":)", "svdft2fdsfy");
INSERT INTO batch VALUES (5, 2, "akacjowy", "2+7", "1l", 20.0, "2010-12-22", "+10", 0.0, "2011-12-22", ":)", "svdft2fdsfy");

INSERT INTO distributor VALUES (1, 1, 1, "2010-06-18", "2010-06-18", 1, 0, 2);
INSERT INTO distributor VALUES (2, 2, 2, "2010-06-18", "2010-06-18", 0, 0, 2);
INSERT INTO distributor VALUES (3, 3, 3, "2010-06-18", "2010-06-18", 1, 0, 2);
INSERT INTO distributor VALUES (4, 1, 4, "2010-06-18", "2010-06-18", 2, 0, 2);
INSERT INTO distributor VALUES (5, 2, 5, "2010-06-18", "2010-06-18", 3, 0, 2);
INSERT INTO distributor VALUES (6, 3, 6, "2010-06-18", "2010-06-18", 2, 0, 2);
INSERT INTO distributor VALUES (7, 1, 1, "2010-06-18", "2010-06-18", 0, 0, 2);
INSERT INTO distributor VALUES (8, 2, 2, "2010-06-18", "2010-06-18", 4, 0, 2);
INSERT INTO distributor VALUES (9, 3, 3, "2010-06-18", "2010-06-18", 0, 0, 2);
INSERT INTO distributor VALUES (10, 4, 4, "2010-06-18", "2010-06-18", 1, 0, 2);
INSERT INTO distributor VALUES (11, 5, 5, "2010-06-18", "2010-06-18", 1, 0, 2);
INSERT INTO distributor VALUES (12, 4, 6, "2010-06-18", "2010-06-18", 1, 0, 2);
INSERT INTO distributor VALUES (13, 5, 1, "2010-06-18", "2010-06-18", 2, 0, 2);
INSERT INTO distributor VALUES (14, 4, 2, "2010-06-18", "2010-06-18", 2, 0, 2);
INSERT INTO distributor VALUES (15, 5, 3, "2010-06-18", "2010-06-18", 0, 0, 2);
INSERT INTO distributor VALUES (16, 4, 4, "2010-06-18", "2010-06-18", 1, 0, 2);
INSERT INTO distributor VALUES (17, 5, 5, "2010-06-18", "2010-06-18", 1, 0, 2);

DELETE FROM settings;
INSERT INTO settings VALUES (1, 1);
INSERT INTO settings VALUES (2, "qwerty");
INSERT INTO settings VALUES (4, "2011-06-18");
INSERT INTO settings VALUES (8, "2011-07-18");
INSERT INTO settings VALUES (16, 10);
INSERT INTO settings VALUES (32, 100);
INSERT INTO settings VALUES (64, "aaa");
INSERT INTO settings VALUES (128, "bbb");
INSERT INTO settings VALUES (256, "ccc");
