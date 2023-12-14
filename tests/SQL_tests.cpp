#include "../lib/MyCoolDB.h"

#include "gtest/gtest.h"

TEST(Data_Base_TestSuite, CreateTableTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                      "    id    INT,"
                      "    name  VARCHAR,"
                      "    list_price   DOUBLE,"
                      "    in_stock  BOOL"
                      ");");

    data_base.Query("INSERT INTO Products(id, name, list_price, in_stock) VALUES (1, 'Milk', 49.99, true)");
    data_base.Query("INSERT INTO Products(id, name) VALUES (2, 'Bread')");
    data_base.Query("INSERT INTO Products(id, name, in_stock) VALUES (3, 'My brain', false)");

    ResultTable result = data_base.Query("SELECT * FROM Products");
    ASSERT_EQ(result.Count(), 3);

    ASSERT_EQ(result.Get("id").GetValueAsInt(), 1);
    ASSERT_EQ(result.Get("name").GetValueAsString(), "Milk");
    ASSERT_EQ(result.Get("list_price").GetValueAsDouble(), 49.99);
    ASSERT_EQ(result.Get("in_stock").GetValueAsBool(), true);

    result.Next();
    ASSERT_EQ(result.Get("id").GetValueAsInt(), 2);
    ASSERT_EQ(result.Get("name").GetValueAsString(), "Bread");
    ASSERT_TRUE(result.Get("list_price").IsNull());
    ASSERT_TRUE(result.Get("in_stock").IsNull());


    result.Next();
    ASSERT_EQ(result.Get("id").GetValueAsInt(), 3);
    ASSERT_EQ(result.Get("name").GetValueAsString(), "My brain");
    ASSERT_TRUE(result.Get("list_price").IsNull());
    ASSERT_EQ(result.Get("in_stock").GetValueAsBool(), false);

    ASSERT_FALSE(result.Next());
}


TEST(Data_Base_TestSuite, SelectTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                           "    id    INT,"
                           "    name  VARCHAR,"
                           "    list_price   DOUBLE,"
                           "    in_stock  BOOL"
                           ");");

    data_base.Query("INSERT INTO Products(id, name, list_price, in_stock) VALUES (1, 'Milk', 49.99, true)");
    data_base.Query("INSERT INTO Products(id, name) VALUES (2, 'Bread')");
    data_base.Query("INSERT INTO Products(id, name, in_stock) VALUES (3, 'My brain', false)");

    ResultTable result = data_base.Query("SELECT id, name, in_stock FROM Products WHERE id < 3");
    ASSERT_EQ(result.Count(), 2);

    ASSERT_EQ(result.Get("id").GetValueAsInt(), 1);
    ASSERT_EQ(result.Get("name").GetValueAsString(), "Milk");
    ASSERT_EQ(result.Get("in_stock").GetValueAsBool(), true);

    result.Next();

    ASSERT_EQ(result.Get("id").GetValueAsInt(), 2);
    ASSERT_EQ(result.Get("name").GetValueAsString(), "Bread");
    ASSERT_EQ(result.Get("in_stock").IsNull(), true);
}



TEST(Data_Base_TestSuite, UpdateTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                           "    id    INT,"
                           "    name  VARCHAR,"
                           "    list_price   DOUBLE,"
                           "    in_stock  BOOL"
                           ");");

    data_base.Query("INSERT INTO Products(id, name, list_price, in_stock) VALUES (1, 'Milk', 49.99, true)");
    data_base.Query("INSERT INTO Products(id, name) VALUES (2, 'Bread')");
    data_base.Query("INSERT INTO Products(id, name, in_stock) VALUES (3, 'My brain', false)");

    data_base.Query("UPDATE Products "
                           "SET list_price = 29.99,"
                           "in_stock = false "
                           "WHERE id = 1");
    data_base.Query("UPDATE Products "
                           "SET in_stock = true "
                           "WHERE id = 3");
    ResultTable result = data_base.Query("SELECT * FROM Products WHERE id = 1 OR id = 3");
    ASSERT_EQ(result.Count(), 2);

    ASSERT_EQ(result.Get("id").GetValueAsInt(), 1);
    ASSERT_EQ(result.Get("list_price").GetValueAsDouble(), 29.99);
    ASSERT_EQ(result.Get("in_stock").GetValueAsBool(), false);

    result.Next();
    ASSERT_EQ(result.Get("id").GetValueAsInt(), 3);
    ASSERT_TRUE(result.Get("in_stock").GetValueAsBool());
}

TEST(Data_Base_TestSuite, DeleteTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                           "    id    INT,"
                           "    name  VARCHAR,"
                           "    list_price   DOUBLE,"
                           "    in_stock  BOOL"
                           ");");

    data_base.Query("INSERT INTO Products(id, name, list_price, in_stock) VALUES (1, 'Milk', 49.99, true)");
    data_base.Query("INSERT INTO Products(id, name) VALUES (2, 'Bread')");
    data_base.Query("INSERT INTO Products(id, name, in_stock) VALUES (3, 'My brain', false)");

    data_base.Query("DELETE FROM Products WHERE id > 2");
    ResultTable result1 = data_base.Query("SELECT * FROM Products");
    ASSERT_EQ(result1.Count(), 2);

    data_base.Query("DELETE FROM Products");
    ResultTable result2 = data_base.Query("SELECT * FROM Products");
    ASSERT_EQ(result2.Count(), 0);
}

TEST(Data_Base_TestSuite, DropTableTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                           "    id    INT,"
                           "    name  VARCHAR,"
                           "    list_price   DOUBLE,"
                           "    in_stock  BOOL"
                           ");");

    data_base.Query("INSERT INTO Products(id, name, list_price, in_stock) VALUES (1, 'Milk', 49.99, true)");
    data_base.Query("INSERT INTO Products(id, name) VALUES (2, 'Bread')");
    data_base.Query("INSERT INTO Products(id, name, in_stock) VALUES (3, 'My brain', false)");

    data_base.Query("CREATE TABLE Customers ("
                      "    money_spent DOUBLE"
                      ")");

    ASSERT_EQ(data_base.GetTableNames(), std::vector<std::string>({"Customers", "Products"}));
    data_base.Query("DROP TABLE Customers");
    ASSERT_EQ(data_base.GetTableNames(), std::vector<std::string>({"Products"}));
    data_base.Query("DROP TABLE Products");
    ASSERT_EQ(data_base.GetTableNames(), std::vector<std::string>{});
}

TEST(Data_Base_TestSuite, PrimaryKeyTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                      "    id INT PRIMARY KEY,"
                      "    name VARCHAR"
                      "    list_price   DOUBLE,"
                      "    in_stock  BOOL"
                      ")");

    data_base.Query("INSERT INTO Products(id, name) VALUES (1, 'Olive oil')");
    ASSERT_THROW(data_base.Query("INSERT INTO Subject(priority, name) VALUES (1, 'Palm oil')"), std::invalid_argument);
}

TEST(Data_Base_TestSuite, ForeignKeyTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                           "    id    INT,"
                           "    name  VARCHAR,"
                           "    list_price   DOUBLE,"
                           "    in_stock  BOOL"
                           ")");

    data_base.Query("INSERT INTO Products(id, name, list_price, in_stock) VALUES (1, 'Milk', 49.99, true)");
    data_base.Query("INSERT INTO Products(id, name) VALUES (2, 'Bread')");
    data_base.Query("INSERT INTO Products(id, name, in_stock) VALUES (3, 'My brain', false)");

    data_base.Query("CREATE TABLE Orders ("
                           "    customer_id INT,"
                           "    number_of_items INT,"
                           "    phone_number VARCHAR,"
                           "    FOREIGN KEY (product_id) REFERENCES Products (id)"
                           ")");

    data_base.Query("INSERT INTO Orders(customer_id, number_of_items, product_id) VALUES (1, 1, 2)");

    ASSERT_EQ(data_base.Query("SELECT * FROM Orders").Count(), 1);
    ResultTable result = data_base.Query("SELECT * FROM Orders");
    ASSERT_EQ(result.Count(), 1);
}


TEST(Data_Base_TestSuite, JoinTest) {
    MyCoolDB data_base;

    data_base.Query("CREATE TABLE Products ("
                           "    id    INT,"
                           "    name  VARCHAR,"
                           "    list_price   DOUBLE,"
                           "    in_stock  BOOL"
                           ");");

    data_base.Query("INSERT INTO Products(id, name, list_price, in_stock) VALUES (1, 'Milk', 49.99, true)");
    data_base.Query("INSERT INTO Products(id, name) VALUES (2, 'Bread')");
    data_base.Query("INSERT INTO Products(id, name, in_stock) VALUES (3, 'My brain', false)");

    data_base.Query("CREATE TABLE Order ("
                           "    product_id INT,"
                           "    customer_id INT,"
                           "    number_of_products INT,"
                           ")");

    data_base.Query("INSERT INTO Order(product_id, customer_id, number_of_products) VALUES (1, 1, 'Break')");
    data_base.Query("INSERT INTO Order(product_id, customer_id, number_of_products) VALUES (2, 1, 'Milk')");

    ResultTable result = data_base.Query(
            "SELECT * FROM Products INNER JOIN Order ON Order.product_id = Products.id");
    ASSERT_EQ(result.Count(), 2);

    ASSERT_EQ(result.Get("Products.id").GetValueAsInt(), result.Get("Order.product_id").GetValueAsInt());
    ASSERT_EQ(result.Get("Products.name").GetValueAsString(), "Milk");
    ASSERT_EQ(result.Get("Products.list_price").GetValueAsDouble(), 49.99);

    result.Next();

    ASSERT_EQ(result.Get("Products.id").GetValueAsInt(), result.Get("Order.product_id").GetValueAsInt());
    ASSERT_EQ(result.Get("Products.name").GetValueAsString(), "Bread");
}