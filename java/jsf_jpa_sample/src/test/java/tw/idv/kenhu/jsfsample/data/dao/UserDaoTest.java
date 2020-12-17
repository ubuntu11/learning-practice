/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.data.dao;

import java.io.FileInputStream;
import org.dbunit.DBTestCase;
import org.dbunit.PropertiesBasedJdbcDatabaseTester;
import org.dbunit.dataset.IDataSet;
import org.dbunit.dataset.xml.FlatXmlDataSetBuilder;
import org.dbunit.operation.DatabaseOperation;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
/**
 *
 * @author ken
 */
public class UserDaoTest extends DBTestCase{
    
    public UserDaoTest(String name)
    {
        super( name );
        System.setProperty( PropertiesBasedJdbcDatabaseTester.DBUNIT_DRIVER_CLASS, "com.mysql.jdbc.Driver" );
        System.setProperty( PropertiesBasedJdbcDatabaseTester.DBUNIT_CONNECTION_URL, "jdbc:mysql://192.168.7.41:3306/jsfsample?zeroDateTimeBehavior=convertToNull&useUnicode=true&characterEncoding=UTF-8" );
        System.setProperty( PropertiesBasedJdbcDatabaseTester.DBUNIT_USERNAME, "jsfsample" );
        System.setProperty( PropertiesBasedJdbcDatabaseTester.DBUNIT_PASSWORD, "jsfsample" );
	// System.setProperty( PropertiesBasedJdbcDatabaseTester.DBUNIT_SCHEMA, "" );
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() throws Exception {
        DatabaseOperation.CLEAN_INSERT.execute(getConnection(), getDataSet());
    }
    
    @After
    public void tearDown() {
    }

    @Override
    protected DatabaseOperation getTearDownOperation() throws Exception
    {
        return DatabaseOperation.DELETE_ALL;
    }

    /**
     * Test of loadAllMessages method, of class UserDao.
     */
    @Test
    public void testLoadAllMessages() {
        System.out.println("loadAllMessages");
        UserDao instance = new UserDao();
        Integer expResult = 3;
        Integer result = instance.loadAllMessages().size();
        assertEquals("there should be 3 records in database", expResult, result);
        
    }

    @Override
    protected IDataSet getDataSet() throws Exception {
        return new FlatXmlDataSetBuilder().build(new FileInputStream("src/test/resources/dataset/guest.xml"));
    }
    
}
