#include <gtest/gtest.h>

extern "C"
{
    #include "package_manager.h"
}

/* cleanup before every test */

class PackageManagerTest : public ::testing::Test
{
protected:

    void SetUp() override
    {
        freePackages();
    }

    void TearDown() override
    {
        freePackages();
    }
};

/*----------------------------------
ADD PACKAGE
-----------------------------------*/

TEST_F(PackageManagerTest, AddPackage)
{
    addPackage("A",1,0,0);

    EXPECT_NE(nullptr,
              findPackage("A"));
}

/*----------------------------------
DUPLICATE PACKAGE
-----------------------------------*/

TEST_F(PackageManagerTest, DuplicatePackage)
{
    addPackage("A",1,0,0);

    addPackage("A",1,0,0);

    EXPECT_NE(nullptr,
              findPackage("A"));
}

/*----------------------------------
SEARCH PACKAGE
-----------------------------------*/

TEST_F(PackageManagerTest, SearchExistingPackage)
{
    addPackage("B",1,0,0);

    EXPECT_NE(nullptr,
              findPackage("B"));
}

TEST_F(PackageManagerTest, SearchMissingPackage)
{
    EXPECT_EQ(nullptr,
              findPackage("XYZ"));
}

/*----------------------------------
UPDATE PACKAGE
-----------------------------------*/

TEST_F(PackageManagerTest, UpdatePackage)
{
    addPackage("C",1,0,0);

    updatePackage("C",2,1,0);

    Package *p =
        findPackage("C");

    ASSERT_NE(nullptr,p);

    EXPECT_EQ(2,
              p->version.major);

    EXPECT_EQ(1,
              p->version.minor);
}

TEST_F(PackageManagerTest,
       UpdateMissingPackage)
{
    EXPECT_NO_THROW(
        updatePackage("AAA",
                      2,0,0));
}

/*----------------------------------
DELETE PACKAGE
-----------------------------------*/

TEST_F(PackageManagerTest, DeletePackage)
{
    addPackage("D",1,0,0);

    deletePackage("D");

    EXPECT_EQ(nullptr,
              findPackage("D"));
}

TEST_F(PackageManagerTest,
       DeleteMissingPackage)
{
    EXPECT_NO_THROW(
        deletePackage("DDD"));
}

/*----------------------------------
ADD DEPENDENCY
-----------------------------------*/

TEST_F(PackageManagerTest,
       AddDependency)
{
    addPackage("APP",1,0,0);
    addPackage("LIB",1,0,0);

    addDependency(
        "APP",
        "LIB",
        1,0,0);

    Package *pkg =
        findPackage("APP");

    ASSERT_NE(nullptr,
              pkg->deps);

    EXPECT_STREQ("LIB",
                 pkg->deps->name);
}

/*----------------------------------
SELF DEPENDENCY
-----------------------------------*/

TEST_F(PackageManagerTest,
       SelfDependency)
{
    addPackage("SELF",1,0,0);

    addDependency(
        "SELF",
        "SELF",
        1,0,0);

    EXPECT_EQ(nullptr,
              findPackage("SELF")->deps);
}

/*----------------------------------
MISSING DEPENDENCY
-----------------------------------*/

TEST_F(PackageManagerTest,
       MissingDependency)
{
    addPackage("APP",1,0,0);

    addDependency(
        "APP",
        "LIB",
        1,0,0);

    EXPECT_EQ(MISSING,
              findPackage("APP")->status);
}

/*----------------------------------
REMOVE DEPENDENCY
-----------------------------------*/

TEST_F(PackageManagerTest,
       RemoveDependency)
{
    addPackage("A",1,0,0);
    addPackage("B",1,0,0);

    addDependency(
        "A",
        "B",
        1,0,0);

    removeDependency(
        "A",
        "B");

    EXPECT_EQ(nullptr,
              findPackage("A")->deps);
}

TEST_F(PackageManagerTest,
       RemoveMissingDependency)
{
    addPackage("A",1,0,0);

    EXPECT_NO_THROW(
        removeDependency(
            "A",
            "XYZ"));
}

/*----------------------------------
VERSION COMPARISON
-----------------------------------*/

TEST_F(PackageManagerTest,
       CompareHigherVersion)
{
    Version v1 = {2,0,0};
    Version v2 = {1,0,0};

    EXPECT_EQ(1,
              compareVersion(v1,v2));
}

TEST_F(PackageManagerTest,
       CompareLowerVersion)
{
    Version v1 = {1,0,0};
    Version v2 = {2,0,0};

    EXPECT_EQ(-1,
              compareVersion(v1,v2));
}

TEST_F(PackageManagerTest,
       CompareEqualVersion)
{
    Version v1 = {1,0,0};
    Version v2 = {1,0,0};

    EXPECT_EQ(0,
              compareVersion(v1,v2));
}

/*----------------------------------
VERSION CONFLICT
-----------------------------------*/

TEST_F(PackageManagerTest,
       VersionConflict)
{
    addPackage("APP",1,0,0);

    addPackage("LIB",1,0,0);

    addDependency(
        "APP",
        "LIB",
        5,0,0);

    checkVersionConflicts();

    EXPECT_EQ(CONFLICTING,
              findPackage("APP")->status);
}

/*----------------------------------
NO CONFLICT
-----------------------------------*/

TEST_F(PackageManagerTest,
       NoVersionConflict)
{
    addPackage("APP",1,0,0);

    addPackage("LIB",5,0,0);

    addDependency(
        "APP",
        "LIB",
        1,0,0);

    checkVersionConflicts();

    EXPECT_NE(CONFLICTING,
              findPackage("APP")->status);
}

/*----------------------------------
CYCLE DETECTION
-----------------------------------*/

TEST_F(PackageManagerTest,
       NoCycle)
{
    addPackage("A",1,0,0);
    addPackage("B",1,0,0);

    addDependency("A",
                  "B",
                  1,0,0);

    resetVisits();

    EXPECT_EQ(
        0,
        detectCycleDFS(
            findPackage("A")));
}

TEST_F(PackageManagerTest,
       DetectCycle)
{
    addPackage("A",1,0,0);
    addPackage("B",1,0,0);

    addDependency("A",
                  "B",
                  1,0,0);

    addDependency("B",
                  "A",
                  1,0,0);

    resetVisits();

    EXPECT_EQ(
        1,
        detectCycleDFS(
            findPackage("A")));
}

/*----------------------------------
INSTALL ORDER
-----------------------------------*/

TEST_F(PackageManagerTest,
       InstallationOrder)
{
    addPackage("APP",1,0,0);
    addPackage("LIB",1,0,0);

    addDependency(
        "APP",
        "LIB",
        1,0,0);

    generateInstallOrder();

    EXPECT_GT(orderIndex,0);
}

/*----------------------------------
FILE HANDLING
-----------------------------------*/

TEST_F(PackageManagerTest,
       SaveFile)
{
    addPackage("A",1,0,0);

    EXPECT_NO_THROW(
        saveToFile());
}

TEST_F(PackageManagerTest,
       LoadFile)
{
    addPackage("A",1,0,0);

    saveToFile();

    freePackages();

    loadFromFile();

    EXPECT_NE(nullptr,
              findPackage("A"));
}
