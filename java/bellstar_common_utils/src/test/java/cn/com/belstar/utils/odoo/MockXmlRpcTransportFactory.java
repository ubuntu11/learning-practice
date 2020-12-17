package cn.com.belstar.utils.odoo;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.XmlRpcTransport;
import org.apache.xmlrpc.client.XmlRpcTransportFactoryImpl;

/**
 * 為了測試xmlrpc所使用的類別,參考:https://www.daganhenderson.com/blog/2015/01/
 * java-xml-rpc-client-apache-xmlrpcclient.
 * @author ken
 *
 */
public class MockXmlRpcTransportFactory extends XmlRpcTransportFactoryImpl {
  protected InputStream response;
  protected MockXmlRpcTransport transport;

  public MockXmlRpcTransportFactory(XmlRpcClient pClient) {
    super(pClient);
    this.transport = new MockXmlRpcTransport(this.getClient());
  }

  @Override
  public XmlRpcTransport getTransport() {
    this.transport = new MockXmlRpcTransport(this.getClient());
    this.transport.setResponse(this.response);
    return this.transport;
  }

  public void setResponse(InputStream stream) {
    this.response = stream;
  }

  public ByteArrayOutputStream getLastRequest() {
    return this.transport.getLastRequest();
  }
}
