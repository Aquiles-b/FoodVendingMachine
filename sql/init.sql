-- Tables creation --------------------------------------------------
CREATE TABLE products (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE,
    price DECIMAL(10, 2) NOT NULL,
    preparation_time INT NOT NULL DEFAULT 5,
    enabled BOOLEAN NOT NULL DEFAULT true
);

CREATE TABLE inventory (
    product_id INT PRIMARY KEY,
    quantity INT NOT NULL DEFAULT 0,

    FOREIGN KEY (product_id) REFERENCES products(id)
);

CREATE TABLE orders (
    id INT AUTO_INCREMENT PRIMARY KEY,
    state VARCHAR(32),
    total_price DECIMAL(10, 2) NOT NULL
);

CREATE TABLE order_items (
    order_id INT NOT NULL,
    product_id INT NOT NULL,
    unit_price DECIMAL NOT NULL,

    FOREIGN KEY(order_id) REFERENCES orders(id),
    FOREIGN KEY(product_id) REFERENCES products(id)
);

CREATE TABLE order_events (
    id INT AUTO_INCREMENT PRIMARY KEY,
    order_id INT,
    event VARCHAR(32),
    time DATETIME,

    FOREIGN KEY (order_id) REFERENCES orders(id)
);

CREATE TABLE statistics (
    metric VARCHAR(64),
    value BIGINT
);

-- ------------------------------------------------------------------


-- EXAMPLE VALUES ---------------------------------------------------
INSERT INTO products(name, price, preparation_time, enabled) VALUES
("Pizza", 20, 15, true),
("Burguer", 15, 8, true),
("Soda", 4, 0, true),
("Hotdog", 10, 10, true),
("Ramen", 5, 3, false);

INSERT INTO inventory(product_id, quantity) VALUES
(1, 20),
(2, 15),
(3, 40),
(4, 25),
(5, 10);

-- ------------------------------------------------------------------
