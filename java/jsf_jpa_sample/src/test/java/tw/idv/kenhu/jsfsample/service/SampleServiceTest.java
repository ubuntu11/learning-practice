/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.service;

import java.util.Arrays;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import tw.idv.kenhu.jsfsample.data.dao.UserDaoIF;
import tw.idv.kenhu.jsfsample.data.entity.GuestMessage;
import static org.mockito.Mockito.*;
/**
 * 用以測試SampeServie的測試類別.
 * @author ken
 */
public class SampleServiceTest {
    /**
     * UserDaoIF mock 實例.
     */
    private UserDaoIF mockDao = mock(UserDaoIF.class);
    /**
     * 受測實例.
     */
    private SampleService instance = null;

    public SampleServiceTest() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
        instance = new SampleService();
        instance.setUserDao(mockDao);
        GuestMessage mesg1 = GuestMessage.builder().name("Ken").message("hello world ken").id(1L).build();
        GuestMessage mesg2 = GuestMessage.builder().name("Kevin").message("hello world kevin").id(2L).build();
        GuestMessage mesg3 = GuestMessage.builder().name("John").message("hello world john").id(3L).build();
        when(mockDao.loadAllMessages()).thenReturn(Arrays.asList(mesg1, mesg2, mesg3));
    }
    
    @After
    public void tearDown() {
    }

    /**
     * Test of sendRequest method, of class SampleService.
     */
    @Test
    public void testSendRequest() {
        System.out.println("sendRequest");
        GuestMessage mockMessage = mock(GuestMessage.class);
        instance.sendMessage(mockMessage);
        verify(mockDao, times(1)).persist(mockMessage);
    }

    /**
     * Test of loadRegisteredUsernames method, of class SampleService.
     */
    @Test
    public void testLoadUsers() {
        System.out.println("testLoadUsers");
        
        Integer expResult = 3;
        Integer result = instance.loadAllMessages().size();
        assertEquals(expResult, result);
    }
    
}
