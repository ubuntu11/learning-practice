package cn.com.belstar.utils.odoo;

import static org.junit.Assert.*;
import java.io.ByteArrayInputStream;
import java.net.URL;
import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.XmlRpcClientConfigImpl;
import org.junit.Test;

public class XmlRpcClientTest {

  @Test
  public void textXmlRpcClientTest() throws Exception {

    // Set up a client config that points no where (we're mocking anyway)
    XmlRpcClientConfigImpl clientConfig = new XmlRpcClientConfigImpl();
    clientConfig.setServerURL(new URL("http://localhost/my/xml-rpc/endpoint"));

    // Create our client
    XmlRpcClient client = new XmlRpcClient();
    client.setConfig(clientConfig);

    // Use our mock transport factories
    MockXmlRpcTransportFactory transportFactory = new MockXmlRpcTransportFactory(client);
    client.setTransportFactory(transportFactory);
    transportFactory.setResponse(new ByteArrayInputStream(("<?xml version=\"1.0\"?>"
        + "<methodResponse>" + "<params>" + "<param>"
        + "<value><double>18.24668429131</double></value>" + "</param>" + "</params>"
        + "</methodResponse>").getBytes()));

    Double answer = (Double) client.execute("add", new Object[] {1234, 56789});
    assertEquals(answer, (Double) 18.24668429131);
  }
}
