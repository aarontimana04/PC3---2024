CREATE TABLE Customers (
    customer_id SERIAL PRIMARY KEY,
    nombre VARCHAR(10),
    email VARCHAR(10)
);

CREATE TABLE Products (
    product_id SERIAL PRIMARY KEY,
    nombre VARCHAR(10),
    price INT,
    stock INT DEFAULT 100
);

CREATE TABLE Orders (
    order_id SERIAL PRIMARY KEY,
    customer_id INT,
    order_date DATE,
    FOREIGN KEY (customer_id) REFERENCES Customers(customer_id)
);

CREATE TABLE OrderDetails (
    order_details_id SERIAL PRIMARY KEY,
    order_id INT,
    product_id INT,
    quantity INT,
    FOREIGN KEY (order_id) REFERENCES Orders(order_id),
    FOREIGN KEY (product_id) REFERENCES Products(product_id)
);

-- a) Vista que muestra el id de orden, nombre del cliente y suma total gastada en esa orden
CREATE VIEW OrderTotals AS
SELECT O.order_id, C.nombre, SUM(P.price * OD.quantity) AS total
FROM Orders O
JOIN Customers C ON O.customer_id = C.customer_id
JOIN OrderDetails OD ON OD.order_id = O.order_id
JOIN Products P ON OD.product_id = P.product_id
GROUP BY O.order_id, C.nombre;

-- b) Crear Trigger que actualice la cantidad en stock después de insertar una fila en OrderDetails
CREATE OR REPLACE FUNCTION actualizar_stock() RETURNS TRIGGER 
AS $$
BEGIN
    UPDATE Products
    SET stock = stock - NEW.quantity
    WHERE product_id = NEW.product_id;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER tg_actualizar
AFTER INSERT ON OrderDetails
FOR EACH ROW
EXECUTE FUNCTION actualizar_stock();

-- Insert sample data
INSERT INTO Customers (nombre, email) VALUES 
('Alice', 'alice@example.com'),
('Bob', 'bob@example.com');

INSERT INTO Products (nombre, price) VALUES 
('Product1', 50),
('Product2', 30),
('Product3', 20);

INSERT INTO Orders (customer_id, order_date) VALUES 
(1, '2024-06-28'),
(2, '2024-06-28');

INSERT INTO OrderDetails (order_id, product_id, quantity) VALUES 
(1, 1, 5),
(1, 2, 10),
(2, 3, 3);

-- Queries to verify
SELECT * FROM Products;
SELECT * FROM OrderTotals;
