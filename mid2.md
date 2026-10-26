```c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

#define DB_NAME "students.db"

void printStudents(sqlite3_stmt* stmt)
{
    int found = 0;

    printf("\n========== 학생 정보 ==========\n");
    printf("%-10s %-12s %-20s %-6s %-6s\n",
        "학번", "이름", "학과", "나이", "학점");
    printf("------------------------------------------------------------\n");

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        printf("%-10d %-12s %-20s %-6d %.1f\n",
            sqlite3_column_int(stmt, 0),
            (const char*)sqlite3_column_text(stmt, 1),
            (const char*)sqlite3_column_text(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4));

        found = 1;
    }

    if (!found)
        printf("검색 결과가 없습니다.\n");

    printf("================================\n");
}

int executeSQL(sqlite3* db, const char* sql)
{
    char* errorMessage = NULL;

    int result = sqlite3_exec(
        db, sql, NULL, NULL, &errorMessage);

    if (result != SQLITE_OK)
    {
        printf("SQL 오류: %s\n",
            errorMessage ? errorMessage : sqlite3_errmsg(db));

        sqlite3_free(errorMessage);
        return 0;
    }

    return 1;
}

void showAllStudents(sqlite3* db)
{
    sqlite3_stmt* stmt = NULL;

    const char* sql =
        "SELECT id, name, department, age, gpa "
        "FROM students ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK)
    {
        printf("조회 오류: %s\n", sqlite3_errmsg(db));
        return;
    }

    printStudents(stmt);
    sqlite3_finalize(stmt);
}

void searchById(sqlite3* db)
{
    int id;

    printf("검색할 학번을 입력하세요: ");

    if (scanf("%d", &id) != 1)
    {
        printf("올바른 학번을 입력하세요.\n");
        while (getchar() != '\n');
        return;
    }

    sqlite3_stmt* stmt = NULL;

    const char* sql =
        "SELECT id, name, department, age, gpa "
        "FROM students WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK)
    {
        printf("검색 오류: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    printStudents(stmt);

    sqlite3_finalize(stmt);
}

void searchByName(sqlite3* db)
{
    char name[100];

    printf("검색할 이름을 입력하세요: ");

    if (scanf("%99s", name) != 1)
    {
        printf("이름 입력 오류입니다.\n");
        while (getchar() != '\n');
        return;
    }

    sqlite3_stmt* stmt = NULL;

    const char* sql =
        "SELECT id, name, department, age, gpa "
        "FROM students WHERE name LIKE ? "
        "ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK)
    {
        printf("검색 오류: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 이름 일부만 입력해도 검색할 수 있음
    char pattern[110];
    snprintf(pattern, sizeof(pattern), "%%%s%%", name);

    sqlite3_bind_text(
        stmt, 1, pattern, -1, SQLITE_TRANSIENT);

    printStudents(stmt);

    sqlite3_finalize(stmt);
}

int main(void)
{
    sqlite3* db = NULL;

    // 데이터베이스 열기 또는 생성
    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK)
    {
        printf("데이터베이스 열기 실패: %s\n",
            sqlite3_errmsg(db));

        sqlite3_close(db);
        return 1;
    }

    printf("학생 정보 데이터베이스 연결 성공!\n");

    // 테이블 생성
    const char* createTable =
        "CREATE TABLE IF NOT EXISTS students ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "department TEXT NOT NULL,"
        "age INTEGER NOT NULL,"
        "gpa REAL NOT NULL"
        ");";

    if (!executeSQL(db, createTable))
    {
        sqlite3_close(db);
        return 1;
    }

    // 초기 데이터 5명 추가
    // 학번이 이미 존재하면 중복 추가하지 않음
    const char* insertData =
        "INSERT OR IGNORE INTO students "
        "(id, name, department, age, gpa) VALUES "
        "(2026001, '김민수', '컴퓨터공학과', 20, 3.8),"
        "(2026002, '이서연', '소프트웨어학과', 21, 4.2),"
        "(2026003, '박준호', '정보통신공학과', 22, 3.5),"
        "(2026004, '최유진', '컴퓨터공학과', 20, 4.0),"
        "(2026005, '정다은', '인공지능학과', 23, 4.3);";

    if (!executeSQL(db, insertData))
    {
        sqlite3_close(db);
        return 1;
    }

    int menu;

    do
    {
        printf("\n");
        printf("================================\n");
        printf("    학생 정보 관리 시스템\n");
        printf("================================\n");
        printf("1. 전체 학생 조회\n");
        printf("2. 학번으로 학생 검색\n");
        printf("3. 이름으로 학생 검색\n");
        printf("0. 종료\n");
        printf("--------------------------------\n");
        printf("메뉴 선택: ");

        if (scanf("%d", &menu) != 1)
        {
            printf("숫자를 입력하세요.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (menu)
        {
        case 1:
            showAllStudents(db);
            break;

        case 2:
            searchById(db);
            break;

        case 3:
            searchByName(db);
            break;

        case 0:
            printf("프로그램을 종료합니다.\n");
            break;

        default:
            printf("잘못된 메뉴입니다. 다시 선택하세요.\n");
        }

    } while (menu != 0);

    // 데이터베이스 연결 종료
    sqlite3_close(db);

    return 0;
}

```
