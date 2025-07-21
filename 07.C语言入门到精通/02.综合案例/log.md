


### **1. 项目结构**
项目分为以下几个部分：
- **Table 结构体**：表示数据库中的表，包含表名、列信息和数据。
- **Column 结构体**：表示表中的列，包含列名和数据类型。
- **Row 结构体**：表示表中的一行数据。
- **Database 结构体**：管理所有表。
- **FileManager 模块**：负责将数据保存到文件和从文件加载数据。
- **Main 函数**：提供用户交互界面，调用数据库功能。

---

### **2. 代码实现**

#### **2.1 头文件定义**
```c
#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_COLUMNS 20
#define MAX_ROWS 1000
#define MAX_STRING_LENGTH 100

// 数据类型枚举
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

// 列结构体
typedef struct {
    char name[MAX_NAME_LENGTH];
    DataType type;
} Column;

// 行结构体
typedef struct {
    void *data[MAX_COLUMNS]; // 存储每列的数据
} Row;

// 表结构体
typedef struct {
    char name[MAX_NAME_LENGTH];
    Column columns[MAX_COLUMNS];
    int columnCount;
    Row rows[MAX_ROWS];
    int rowCount;
} Table;

// 数据库结构体
typedef struct {
    Table tables[MAX_COLUMNS];
    int tableCount;
} Database;

// 函数声明
void createTable(Database *db, const char *tableName, Column *columns, int columnCount);
void insertRow(Database *db, const char *tableName, void **data);
void deleteRow(Database *db, const char *tableName, int rowIndex);
void updateRow(Database *db, const char *tableName, int rowIndex, void **data);
void queryTable(Database *db, const char *tableName);
void saveDatabase(Database *db, const char *filename);
void loadDatabase(Database *db, const char *filename);

#endif // DATABASE_H
```

---

#### **2.2 数据库操作实现**
```c
#include "database.h"

// 创建表
void createTable(Database *db, const char *tableName, Column *columns, int columnCount) {
    if (db->tableCount >= MAX_COLUMNS) {
        printf("无法创建表：数据库表数量已达上限！\n");
        return;
    }

    Table *table = &db->tables[db->tableCount];
    strncpy(table->name, tableName, MAX_NAME_LENGTH);
    table->columnCount = columnCount;
    for (int i = 0; i < columnCount; i++) {
        strncpy(table->columns[i].name, columns[i].name, MAX_NAME_LENGTH);
        table->columns[i].type = columns[i].type;
    }
    table->rowCount = 0;
    db->tableCount++;

    printf("表 '%s' 创建成功！\n", tableName);
}

// 插入行
void insertRow(Database *db, const char *tableName, void **data) {
    Table *table = NULL;
    for (int i = 0; i < db->tableCount; i++) {
        if (strcmp(db->tables[i].name, tableName) == 0) {
            table = &db->tables[i];
            break;
        }
    }

    if (!table) {
        printf("表 '%s' 不存在！\n", tableName);
        return;
    }

    if (table->rowCount >= MAX_ROWS) {
        printf("无法插入行：表 '%s' 行数已达上限！\n", tableName);
        return;
    }

    Row *row = &table->rows[table->rowCount];
    for (int i = 0; i < table->columnCount; i++) {
        switch (table->columns[i].type) {
            case TYPE_INT:
                row->data[i] = malloc(sizeof(int));
                *(int *)row->data[i] = *(int *)data[i];
                break;
            case TYPE_FLOAT:
                row->data[i] = malloc(sizeof(float));
                *(float *)row->data[i] = *(float *)data[i];
                break;
            case TYPE_STRING:
                row->data[i] = malloc(MAX_STRING_LENGTH);
                strncpy((char *)row->data[i], (char *)data[i], MAX_STRING_LENGTH);
                break;
        }
    }
    table->rowCount++;

    printf("行插入成功！\n");
}

// 删除行
void deleteRow(Database *db, const char *tableName, int rowIndex) {
    Table *table = NULL;
    for (int i = 0; i < db->tableCount; i++) {
        if (strcmp(db->tables[i].name, tableName) == 0) {
            table = &db->tables[i];
            break;
        }
    }

    if (!table) {
        printf("表 '%s' 不存在！\n", tableName);
        return;
    }

    if (rowIndex < 0 || rowIndex >= table->rowCount) {
        printf("行索引无效！\n");
        return;
    }

    for (int i = rowIndex; i < table->rowCount - 1; i++) {
        table->rows[i] = table->rows[i + 1];
    }
    table->rowCount--;

    printf("行删除成功！\n");
}

// 更新行
void updateRow(Database *db, const char *tableName, int rowIndex, void **data) {
    Table *table = NULL;
    for (int i = 0; i < db->tableCount; i++) {
        if (strcmp(db->tables[i].name, tableName) == 0) {
            table = &db->tables[i];
            break;
        }
    }

    if (!table) {
        printf("表 '%s' 不存在！\n", tableName);
        return;
    }

    if (rowIndex < 0 || rowIndex >= table->rowCount) {
        printf("行索引无效！\n");
        return;
    }

    Row *row = &table->rows[rowIndex];
    for (int i = 0; i < table->columnCount; i++) {
        switch (table->columns[i].type) {
            case TYPE_INT:
                *(int *)row->data[i] = *(int *)data[i];
                break;
            case TYPE_FLOAT:
                *(float *)row->data[i] = *(float *)data[i];
                break;
            case TYPE_STRING:
                strncpy((char *)row->data[i], (char *)data[i], MAX_STRING_LENGTH);
                break;
        }
    }

    printf("行更新成功！\n");
}

// 查询表
void queryTable(Database *db, const char *tableName) {
    Table *table = NULL;
    for (int i = 0; i < db->tableCount; i++) {
        if (strcmp(db->tables[i].name, tableName) == 0) {
            table = &db->tables[i];
            break;
        }
    }

    if (!table) {
        printf("表 '%s' 不存在！\n", tableName);
        return;
    }

    printf("表 '%s' 数据：\n", tableName);
    for (int i = 0; i < table->columnCount; i++) {
        printf("%-20s", table->columns[i].name);
    }
    printf("\n");

    for (int i = 0; i < table->rowCount; i++) {
        Row *row = &table->rows[i];
        for (int j = 0; j < table->columnCount; j++) {
            switch (table->columns[j].type) {
                case TYPE_INT:
                    printf("%-20d", *(int *)row->data[j]);
                    break;
                case TYPE_FLOAT:
                    printf("%-20.2f", *(float *)row->data[j]);
                    break;
                case TYPE_STRING:
                    printf("%-20s", (char *)row->data[j]);
                    break;
            }
        }
        printf("\n");
    }
}
```

---

#### **2.3 文件操作实现**
```c
#include "database.h"

// 保存数据库到文件
void saveDatabase(Database *db, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("无法打开文件 '%s'！\n", filename);
        return;
    }

    fwrite(&db->tableCount, sizeof(int), 1, file);
    for (int i = 0; i < db->tableCount; i++) {
        Table *table = &db->tables[i];
        fwrite(table->name, sizeof(char), MAX_NAME_LENGTH, file);
        fwrite(&table->columnCount, sizeof(int), 1, file);
        fwrite(table->columns, sizeof(Column), MAX_COLUMNS, file);
        fwrite(&table->rowCount, sizeof(int), 1, file);
        for (int j = 0; j < table->rowCount; j++) {
            Row *row = &table->rows[j];
            for (int k = 0; k < table->columnCount; k++) {
                switch (table->columns[k].type) {
                    case TYPE_INT:
                        fwrite(row->data[k], sizeof(int), 1, file);
                        break;
                    case TYPE_FLOAT:
                        fwrite(row->data[k], sizeof(float), 1, file);
                        break;
                    case TYPE_STRING:
                        fwrite(row->data[k], sizeof(char), MAX_STRING_LENGTH, file);
                        break;
                }
            }
        }
    }

    fclose(file);
    printf("数据库保存成功！\n");
}

// 从文件加载数据库
void loadDatabase(Database *db, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("无法打开文件 '%s'！\n", filename);
        return;
    }

    fread(&db->tableCount, sizeof(int), 1, file);
    for (int i = 0; i < db->tableCount; i++) {
        Table *table = &db->tables[i];
        fread(table->name, sizeof(char), MAX_NAME_LENGTH, file);
        fread(&table->columnCount, sizeof(int), 1, file);
        fread(table->columns, sizeof(Column), MAX_COLUMNS, file);
        fread(&table->rowCount, sizeof(int), 1, file);
        for (int j = 0; j < table->rowCount; j++) {
            Row *row = &table->rows[j];
            for (int k = 0; k < table->columnCount; k++) {
                switch (table->columns[k].type) {
                    case TYPE_INT:
                        row->data[k] = malloc(sizeof(int));
                        fread(row->data[k], sizeof(int), 1, file);
                        break;
                    case TYPE_FLOAT:
                        row->data[k] = malloc(sizeof(float));
                        fread(row->data[k], sizeof(float), 1, file);
                        break;
                    case TYPE_STRING:
                        row->data[k] = malloc(MAX_STRING_LENGTH);
                        fread(row->data[k], sizeof(char), MAX_STRING_LENGTH, file);
                        break;
                }
            }
        }
    }

    fclose(file);
    printf("数据库加载成功！\n");
}
```

---

#### **2.4 Main 函数**
```c
#include "database.h"

int main() {
    Database db = {0};
    const char *filename = "database.dat";

    // 示例：创建表
    Column columns[] = {
        {"ID", TYPE_INT},
        {"Name", TYPE_STRING},
        {"Age", TYPE_INT}
    };
    createTable(&db, "Users", columns, 3);

    // 示例：插入行
    void *row1[] = {&(int){1}, "Alice", &(int){25}};
    void *row2[] = {&(int){2}, "Bob", &(int){30}};
    insertRow(&db, "Users", row1);
    insertRow(&db, "Users", row2);

    // 示例：查询表
    queryTable(&db, "Users");

    // 示例：保存数据库
    saveDatabase(&db, filename);

    // 示例：加载数据库
    Database newDb = {0};
    loadDatabase(&newDb, filename);
    queryTable(&newDb, "Users");

    return 0;
}
```

---

### **3. 总结**
该案例实现了一个简易的基于文件的数据库系统，涵盖了表结构创建、数据插入、删除、更新、查询以及文件读写等功能。通过合理的数据结构和模块化设计，代码具有良好的可读性和扩展性。




### **3. 详细设计**

#### **3.1 数据结构设计**
```c
// 数据类型枚举
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

// 列结构体
typedef struct {
    char name[MAX_NAME_LENGTH]; // 列名
    DataType type;              // 数据类型
} Column;

// 行结构体
typedef struct {
    void *data[MAX_COLUMNS]; // 存储每列的数据
} Row;

// 表结构体
typedef struct {
    char name[MAX_NAME_LENGTH]; // 表名
    Column columns[MAX_COLUMNS]; // 列信息
    int columnCount;            // 列数
    Row rows[MAX_ROWS];         // 行数据
    int rowCount;               // 行数
} Table;

// 数据库结构体
typedef struct {
    Table tables[MAX_COLUMNS]; // 所有表
    int tableCount;            // 表数
} Database;
```

#### **3.2 核心功能实现**
- **创建表**：
  ```c
  void createTable(Database *db, const char *tableName, Column *columns, int columnCount);
  ```
    - 检查表数量是否超过上限。
    - 初始化表结构，将表添加到数据库中。

- **插入行**：
  ```c
  void insertRow(Database *db, const char *tableName, void **data);
  ```
    - 根据表名找到目标表。
    - 检查行数是否超过上限。
    - 将数据插入到表的行数组中。

- **删除行**：
  ```c
  void deleteRow(Database *db, const char *tableName, int rowIndex);
  ```
    - 根据表名找到目标表。
    - 检查行索引是否有效。
    - 将后续行前移，覆盖被删除的行。

- **更新行**：
  ```c
  void updateRow(Database *db, const char *tableName, int rowIndex, void **data);
  ```
    - 根据表名找到目标表。
    - 检查行索引是否有效。
    - 更新指定行的数据。

- **查询表**：
  ```c
  void queryTable(Database *db, const char *tableName);
  ```
    - 根据表名找到目标表。
    - 遍历表的所有行，打印数据。

#### **3.3 文件操作实现**
- **保存数据库到文件**：
  ```c
  void saveDatabase(Database *db, const char *filename);
  ```
    - 打开文件，将数据库中的表结构和数据写入文件。

- **从文件加载数据库**：
  ```c
  void loadDatabase(Database *db, const char *filename);
  ```
    - 打开文件，读取表结构和数据，初始化数据库。

#### **3.4 用户交互实现**
- **主菜单**：
  ```c
  void displayMenu();
  ```
    - 显示功能选项，接收用户输入。

- **主函数**：
  ```c
  int main();
  ```
    - 初始化数据库，调用功能函数。

---

### **4. 扩展性**
- **支持更多数据类型**：可以扩展 `DataType` 枚举，支持更多数据类型（如日期、布尔值等）。
- **支持更多操作**：可以添加更多功能（如修改表结构、删除表等）。
- **支持更多文件格式**：可以扩展文件操作模块，支持 JSON、XML 等文件格式。

---

### **5. 总结**
该案例通过模块化设计和面向对象思想，实现了一个基于文件的简易数据库系统。通过合理的数据结构和文件操作，代码具有良好的可读性、可维护性和扩展性。

在这个基于文件的简易数据库系统的案例中，涉及了多个技术点，涵盖了数据结构、文件操作、内存管理、用户交互等方面。以下是详细的技术点说明：

---

### **1. 数据结构**
#### **1.1 结构体（Struct）**
- 使用结构体定义数据库的核心数据结构：
    - `Column`：表示表中的列，包含列名和数据类型。
    - `Row`：表示表中的一行数据，使用指针数组存储每列的数据。
    - `Table`：表示表，包含表名、列信息、行数据和行数。
    - `Database`：表示数据库，包含多个表和表数。

#### **1.2 枚举（Enum）**
- 使用枚举定义数据类型：
  ```c
  typedef enum {
      TYPE_INT,
      TYPE_FLOAT,
      TYPE_STRING
  } DataType;
  ```
  用于表示列的数据类型（如整数、浮点数、字符串）。

#### **1.3 数组**
- 使用数组存储多列和多行数据：
    - `Column columns[MAX_COLUMNS]`：存储表中的列信息。
    - `Row rows[MAX_ROWS]`：存储表中的行数据。

---

### **2. 文件操作**
#### **2.1 文件读写**
- 使用 `fopen`、`fread`、`fwrite`、`fclose` 等函数实现文件的读写操作。
- 将数据库的表结构和数据保存到文件中，并从文件加载数据。

#### **2.2 二进制文件**
- 使用二进制格式存储数据，确保高效读写：
  ```c
  FILE *file = fopen(filename, "wb"); // 写二进制文件
  FILE *file = fopen(filename, "rb"); // 读二进制文件
  ```

#### **2.3 数据序列化与反序列化**
- **序列化**：将数据库中的表结构和数据转换为二进制格式，写入文件。
- **反序列化**：从文件中读取二进制数据，还原为数据库的表结构和数据。

---

### **3. 内存管理**
#### **3.1 动态内存分配**
- 使用 `malloc` 动态分配内存，存储行数据：
  ```c
  row->data[i] = malloc(sizeof(int)); // 分配整数内存
  row->data[i] = malloc(MAX_STRING_LENGTH); // 分配字符串内存
  ```

#### **3.2 内存释放**
- 使用 `free` 释放动态分配的内存，避免内存泄漏：
  ```c
  free(row->data[i]);
  ```

---

### **4. 字符串处理**
#### **4.1 字符串操作**
- 使用 `strncpy` 复制字符串，确保字符串长度不超过限制：
  ```c
  strncpy(table->name, tableName, MAX_NAME_LENGTH);
  ```

#### **4.2 字符串比较**
- 使用 `strcmp` 比较字符串，查找目标表：
  ```c
  if (strcmp(db->tables[i].name, tableName) == 0) {
      // 找到目标表
  }
  ```

---

### **5. 用户交互**
#### **5.1 命令行界面（CLI）**
- 通过命令行与用户交互，提供功能选项：
  ```c
  printf("1. 创建表\n");
  printf("2. 插入行\n");
  printf("3. 删除行\n");
  printf("4. 更新行\n");
  printf("5. 查询表\n");
  printf("6. 保存数据库\n");
  printf("7. 加载数据库\n");
  ```

#### **5.2 输入处理**
- 使用 `scanf` 和 `getchar` 接收用户输入：
  ```c
  int choice;
  scanf("%d", &choice);
  ```

---

### **6. 错误处理**
#### **6.1 空指针检查**
- 在查找表时，检查返回的指针是否为空：
  ```c
  if (!table) {
      printf("表 '%s' 不存在！\n", tableName);
      return;
  }
  ```

#### **6.2 文件打开检查**
- 在读写文件时，检查文件是否成功打开：
  ```c
  FILE *file = fopen(filename, "rb");
  if (!file) {
      printf("无法打开文件 '%s'！\n", filename);
      return;
  }
  ```

#### **6.3 输入验证**
- 检查用户输入的行索引是否有效：
  ```c
  if (rowIndex < 0 || rowIndex >= table->rowCount) {
      printf("行索引无效！\n");
      return;
  }
  ```

---

### **7. 循环和条件语句**
#### **7.1 循环**
- 使用 `for` 循环遍历表和行：
  ```c
  for (int i = 0; i < db->tableCount; i++) {
      // 遍历表
  }
  ```

- 使用 `while` 循环实现主菜单的持续显示：
  ```c
  while (1) {
      // 显示菜单
  }
  ```

#### **7.2 条件语句**
- 使用 `if-else` 和 `switch` 语句根据用户选择执行不同的操作：
  ```c
  switch (choice) {
      case 1:
          // 创建表
          break;
      case 2:
          // 插入行
          break;
      // 其他选项
  }
  ```

---

### **8. 模块化设计**
#### **8.1 函数封装**
- 将功能封装到独立的函数中，提高代码的可读性和可维护性：
  ```c
  void createTable(Database *db, const char *tableName, Column *columns, int columnCount);
  void insertRow(Database *db, const char *tableName, void **data);
  void deleteRow(Database *db, const char *tableName, int rowIndex);
  void updateRow(Database *db, const char *tableName, int rowIndex, void **data);
  void queryTable(Database *db, const char *tableName);
  void saveDatabase(Database *db, const char *filename);
  void loadDatabase(Database *db, const char *filename);
  ```

#### **8.2 头文件**
- 使用头文件声明函数和数据结构，便于模块化设计：
  ```c
  #ifndef DATABASE_H
  #define DATABASE_H
  // 函数和结构体声明
  #endif // DATABASE_H
  ```

---

### **9. 总结**
该案例涉及了以下技术点：
1. **数据结构**：结构体、枚举、数组。
2. **文件操作**：文件读写、二进制文件、序列化与反序列化。
3. **内存管理**：动态内存分配与释放。
4. **字符串处理**：字符串复制与比较。
5. **用户交互**：命令行界面、输入处理。
6. **错误处理**：空指针检查、文件打开检查、输入验证。
7. **循环和条件语句**：`for` 循环、`while` 循环、`if-else`、`switch`。
8. **模块化设计**：函数封装、头文件。

通过这些技术点的结合，实现了一个功能完善的简易数据库系统。
