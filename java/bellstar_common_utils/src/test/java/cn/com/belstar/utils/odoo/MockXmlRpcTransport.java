package cn.com.belstar.utils.odoo;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import org.apache.xmlrpc.XmlRpcException;
import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.XmlRpcClientException;
import org.apache.xmlrpc.client.XmlRpcStreamTransport;
import org.apache.xmlrpc.common.XmlRpcStreamRequestConfig;
import org.xml.sax.SAXException;

/**
 * 為了測試xmlrpc所使用的類別,參考:https://www.daganhenderson.com/blog/2015/01/
 * java-xml-rpc-client-apache-xmlrpcclient.
 * @author ken
 *
 */
public class MockXmlRpcTransport extends XmlRpcStreamTransport {
  protected ByteArrayOutputStream request;
  protected InputStream response;

  public MockXmlRpcTransport(XmlRpcClient client) {
    super(client);
  }

  public MockXmlRpcTransport(XmlRpcClient client, InputStream response) {
    super(client);
    this.setResponse(response);
  }

  public ByteArrayOutputStream getLastRequest() {
    return this.request;
  }

  public InputStream getResponse() {
    return this.response;
  }

  public void setResponse(InputStream response) {
    this.response = response;
  }

  @Override
  protected void close() throws XmlRpcClientException {
    return;
  }

  @Override
  protected boolean isResponseGzipCompressed(XmlRpcStreamRequestConfig pConfig) {
    return false;
  }

  @Override
  protected InputStream getInputStream() throws XmlRpcException {
    return this.response;
  }

  @Override
  protected void writeRequest(ReqWriter pWriter) throws XmlRpcException, IOException, SAXException {

    this.request = new ByteArrayOutputStream();
    pWriter.write(this.request);
  }

}
