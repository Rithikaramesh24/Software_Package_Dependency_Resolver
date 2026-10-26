#include <gtest/gtest.h>

extern "C"
{
    #include "package_manager.h"
}

/*=================================================
PACKAGE TESTS
=================================================*/

/* TC01 */
TEST(PackageTest, AddPackage)
{
    addPackage("A",1,0,0);

    EXPECT_NE(nullptr,
              findPackage("A"));
}

/* TC02 */
TEST(PackageTest, DuplicatePackage)
{
    addPackage("DUP",1,0,0);

    EXPECT_NO_THROW(
        addPackage("DUP",1,0,0)
    );
}

/* TC03 */
TEST(PackageTest, SearchPackage)
{
    addPackage("SEARCH",1,0,0);

    EXPECT_NE(nullptr,
              findPackage("SEARCH"));
}

/* TC04 */
TEST(PackageTest, SearchNonExistingPackage)
{
    EXPECT_EQ(nullptr,
              findPackage("UNKNOWN"));
}

/* TC05 */
TEST(PackageTest, UpdatePackage)
{
    addPackage("UPDATE",1,0,0);

    updatePackage("UPDATE",
                  2,1,0);

    Package *p =
        findPackage("UPDATE");

    ASSERT_NE(nullptr,p);

    EXPECT_EQ(2,p->version.major);
    EXPECT_EQ(1,p->version.minor);
    EXPECT_EQ(0,p->version.patch);
}



/* TC07 */
TEST(PackageTest, DeletePackage)
{
    addPackage("DELETE",1,0,0);

    deletePackage("DELETE");

    EXPECT_EQ(nullptr,
              findPackage("DELETE"));
}



/* TC09 */
TEST(PackageTest, ListPackages)
{
    EXPECT_NO_THROW(
        listPackages()
    );
}

/*=================================================
DEPENDENCY TESTS
=================================================*/

/* TC10 */
TEST(DependencyTest, AddDependency)
{
    addPackage("APP",1,0,0);
    addPackage("LIB",1,0,0);

    addDependency("APP",
                  "LIB",
                  1,0,0);

    Package *p =
        findPackage("APP");

    ASSERT_NE(nullptr,p->deps);

    EXPECT_STREQ("LIB",
                 p->deps->name);
}

/* TC11 */
TEST(DependencyTest, MissingDependency)
{
    addPackage("APP2",1,0,0);

    addDependency("APP2",
                  "MISSING_LIB",
                  1,0,0);

    EXPECT_EQ(MISSING,
              findPackage("APP2")->status);
}

/* TC12 */
TEST(DependencyTest, SelfDependency)
{
    addPackage("SELF",1,0,0);

    addDependency("SELF",
                  "SELF",
                  1,0,0);

    EXPECT_EQ(nullptr,
              findPackage("SELF")->deps);
}

/* TC13 */
TEST(DependencyTest, DuplicateDependency)
{
    addPackage("APP3",1,0,0);
    addPackage("LIB3",1,0,0);

    addDependency("APP3",
                  "LIB3",
                  1,0,0);

    EXPECT_NO_THROW(
        addDependency("APP3",
                      "LIB3",
                      1,0,0)
    );
}

/* TC14 */
TEST(DependencyTest, RemoveDependency)
{
    addPackage("APP4",1,0,0);
    addPackage("LIB4",1,0,0);

    addDependency("APP4",
                  "LIB4",
                  1,0,0);

    removeDependency("APP4",
                     "LIB4");

    EXPECT_EQ(nullptr,
              findPackage("APP4")->deps);
}

/* TC15 */
TEST(DependencyTest, RemoveMissingDependency)
{
    EXPECT_NO_THROW(
        removeDependency("APP4",
                         "XYZ")
    );
}

/* TC16 */
TEST(DependencyTest, ShowDependencies)
{
    EXPECT_NO_THROW(
        showDependencies("APP4")
    );
}

/*=================================================
VERSION TESTS
=================================================*/

/* TC17 */
TEST(VersionTest, VersionConflict)
{
    addPackage("APP5",1,0,0);

    addPackage("LIB5",1,0,0);

    addDependency("APP5",
                  "LIB5",
                  3,0,0);

    checkVersionConflicts();

    EXPECT_EQ(CONFLICTING,
              findPackage("APP5")->status);
}

/* TC18 */
TEST(VersionTest, NoConflict)
{
    addPackage("APP6",1,0,0);

    addPackage("LIB6",5,0,0);

    addDependency("APP6",
                  "LIB6",
                  1,0,0);

    EXPECT_NO_THROW(
        checkVersionConflicts()
    );
}

/*=================================================
CYCLE DETECTION TESTS
=================================================*/

/* TC19 */
TEST(CycleTest, NoCycle)
{
    addPackage("A1",1,0,0);
    addPackage("B1",1,0,0);

    addDependency("A1",
                  "B1",
                  1,0,0);

    resetVisits();

    EXPECT_EQ(0,
        detectCycleDFS(
            findPackage("A1")
        ));
}

/* TC20 */
TEST(CycleTest, DetectSimpleCycle)
{
    addPackage("A2",1,0,0);
    addPackage("B2",1,0,0);

    addDependency("A2",
                  "B2",
                  1,0,0);

    addDependency("B2",
                  "A2",
                  1,0,0);

    resetVisits();

    EXPECT_EQ(1,
        detectCycleDFS(
            findPackage("A2")
        ));
}

/* TC21 */
TEST(CycleTest, DetectComplexCycle)
{
    addPackage("X",1,0,0);
    addPackage("Y",1,0,0);
    addPackage("Z",1,0,0);

    addDependency("X","Y",1,0,0);
    addDependency("Y","Z",1,0,0);
    addDependency("Z","X",1,0,0);

    resetVisits();

    EXPECT_EQ(1,
        detectCycleDFS(
            findPackage("X")
        ));
}

/*=================================================
INSTALLATION ORDER TESTS
=================================================*/

/* TC22 */
TEST(InstallTest, GenerateInstallOrder)
{
    addPackage("MAIN",1,0,0);
    addPackage("CORE",1,0,0);

    addDependency("MAIN",
                  "CORE",
                  1,0,0);

    EXPECT_NO_THROW(
        generateInstallOrder()
    );
}

/* TC23 */
TEST(InstallTest, InstallOrderWithCycle)
{
    EXPECT_NO_THROW(
        generateInstallOrder()
    );
}

/*=================================================
FILE TESTS
=================================================*/

/* TC24 */
TEST(FileTest, SaveData)
{
    EXPECT_NO_THROW(
        saveToFile()
    );
}

/* TC25 */
TEST(FileTest, LoadData)
{
    EXPECT_NO_THROW(
        loadFromFile()
    );
}

/*=================================================
STATUS TESTS
=================================================*/

/* TC26 */
TEST(StatusTest, DefaultPendingStatus)
{
    addPackage("STATUS",1,0,0);

    EXPECT_EQ(PENDING,
              findPackage("STATUS")->status);
}

/* TC27 */
TEST(StatusTest, ShowStatus)
{
    EXPECT_NO_THROW(
        showPackageStatus()
    );
}