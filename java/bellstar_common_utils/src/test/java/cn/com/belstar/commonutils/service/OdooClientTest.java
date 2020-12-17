package cn.com.belstar.commonutils.service;

import static org.junit.Assert.*;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Collections;
import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.XmlRpcClientConfigImpl;
import org.junit.Before;
import org.junit.Test;
import cn.com.belstar.commonutils.utils.odoo.Filter;
import cn.com.belstar.utils.odoo.MockXmlRpcTransportFactory;

/**
 * 對OdooClient做單元測試的測試類別.
 * @author ken
 *
 */
public class OdooClientTest {
  /**
   * Odoo驗證使用者的回傳XML檔名.
   */
  private static final String ODOO_AUTHENTICATE_XML = "authenticate.xml";
  /**
   * 查詢Odoo並回傳物件陣列的XML檔名.
   */
  private static final String ODOO_QUERY_OBJECT_XML = "query_object.xml";
  /**
   * 查詢Odoo並回傳物件的XML檔名.
   */
  private static final String ODOO_QUERY_OBJECT_ARRAY_XML = "query_object_array.xml";

  /**
   * 為了單元測試,而繼承並覆寫getXmlRpcClient() method的類別.
   * @author ken
   *
   */
  private class TestOdooClient extends OdooClient {
    /**
     * 作為Odoo回傳資料的XML檔名.
     */
    private String odooDataQueryXml = "";

    /**
     * Constructor.
     * @param odooUrl odoo url
     * @param odooDatabase database name
     * @param odooUsername user name
     * @param odooPasword password
     */
    TestOdooClient(String odooUrl, String odooDatabase, String odooUsername,
        String odooPasword) {
      super(odooUrl, odooDatabase, odooUsername, odooPasword);
    }

    @Override
    protected XmlRpcClient getXmlRpcClient(String urlPattern) throws MalformedURLException {
      // Set up a client config that points no where (we're mocking anyway)
      XmlRpcClientConfigImpl clientConfig = new XmlRpcClientConfigImpl();
      clientConfig.setServerURL(new URL("http://localhost/my/xml-rpc/endpoint"));

      // Create our client
      XmlRpcClient xmlClient = new XmlRpcClient();
      xmlClient.setConfig(clientConfig);

      // Use our mock transport factories
      MockXmlRpcTransportFactory transportFactory = new MockXmlRpcTransportFactory(xmlClient);
      xmlClient.setTransportFactory(transportFactory);
      if (urlPattern.contains("common")) {
        try {
          transportFactory.setResponse(new FileInputStream(new File(this.getClass()
              .getResource(ODOO_AUTHENTICATE_XML).getFile())));
        } catch (FileNotFoundException e) {
          e.printStackTrace();
        }
      } else {
        try {
          transportFactory.setResponse(new FileInputStream(new File(this.getClass()
              .getResource(odooDataQueryXml).getFile())));
        } catch (FileNotFoundException e) {
          // TODO Auto-generated catch block
          e.printStackTrace();
        }
      }

      return xmlClient;
    }

    public void setOdooDataQueryXml(String odooDataQueryXml) {
      this.odooDataQueryXml = odooDataQueryXml;
    }
  }

  /**
   * 受測對象.
   */
  private OdooClient odooClient;

  /**
   * 每個測試案例執行前的設定動作.
   * @throws MalformedURLException if url is malformed
   */
  @Before
  public void setUp() throws MalformedURLException {
    odooClient = new TestOdooClient("", "", "", "");
  }

  /**
   * Test read() method.
   * @throws Exception if anything goes wrong
   */
  @Test
  public void testRead() throws Exception {
    ((TestOdooClient) odooClient).setOdooDataQueryXml(ODOO_QUERY_OBJECT_XML);
    String result = (String) odooClient.read("add", 0, Filter.builder().build());
    assertEquals("should get a String belstar", "belstar", result);
  }

  /**
   * 測試對Odoo進行查詢且預期返回物件陣列的方法.
   * @throws Exception if anything goes wrong
   */
  @Test
  public void testSearchRead() throws Exception {
    ((TestOdooClient) odooClient).setOdooDataQueryXml(ODOO_QUERY_OBJECT_ARRAY_XML);
    @SuppressWarnings("unchecked")
    Object[] result =
        odooClient.searchRead("add", Collections.EMPTY_LIST, Filter.builder().build());
    assertNotNull(result);
    assertEquals("array should contain 2 elements", 2, result.length);
    assertEquals("first element should be '1.2.3.4'", "1.2.3.4", (String) result[0]);
    assertEquals("first element should be '2.2.2.2'", "2.2.2.2", (String) result[1]);
  }

}
