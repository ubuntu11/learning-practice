package cn.com.belstar.commonutils.service;

import java.io.IOException;
import java.security.KeyManagementException;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.HashMap;
import java.util.Map;
import javax.net.ssl.SSLContext;
import javax.servlet.http.HttpServletResponse;
import org.apache.chemistry.opencmis.client.api.Session;
import org.apache.chemistry.opencmis.client.api.SessionFactory;
import org.apache.chemistry.opencmis.client.runtime.SessionFactoryImpl;
import org.apache.chemistry.opencmis.commons.SessionParameter;
import org.apache.chemistry.opencmis.commons.enums.BindingType;
import org.apache.commons.io.IOUtils;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.conn.ssl.NoopHostnameVerifier;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.ssl.SSLContextBuilder;
import org.apache.http.ssl.TrustStrategy;
import org.apache.log4j.Logger;
import com.github.cliftonlabs.json_simple.JsonKey;
import com.github.cliftonlabs.json_simple.JsonObject;
import com.github.cliftonlabs.json_simple.Jsoner;

/**
 * 用來登人、存取Alfresco系統所使用的類別.
 * @author ken
 *
 */
public class AlfrescoClient {
  /**
   * 取得Alfresco Login Session的URI.
   */
  private static final String LOGIN_SESSION_URI =
      "/alfresco/api/-default-/public/cmis/versions/1.1/atom";
  /**
   * 產生Alfresco生成Login Ticket的URL的字串樣式.
   */
  private static final String LOGIN_TICKET_PATTERN =
      "%s/alfresco/s/api/login?u=%s&pw=%s&format=json";
  /**
   * Log4j logger instance.
   */
  private Logger logger = Logger.getLogger(AlfrescoClient.class);
  /**
   * Alfresco系統的基本url,包含通訊協定,host name或ip address以及埠號(optional).
   */
  private String baseUrl;
  /**
   * 登入Alfresco系統所使用的使用者名稱.
   */
  private String username;
  /**
   * 登入Alfresco系統所使用的使用者密碼.
   */
  private String password;
  /**
   * Default JsonObject instance in order to prevent deserialization failure.
   */
  private JsonObject defaultJsonObject;
  /**
   * Default constructor.
   * @param baseUrl Alfresco系統的基本url
   * @param username 使用者名稱
   * @param password 使用者密碼
   */
  public AlfrescoClient(String baseUrl, String username, String password) {
    this.baseUrl = baseUrl;
    this.username = username;
    this.password = password;

    init();
  }
  /**
   * Initialization procedures.
   */
  private void init() {
    Map<String, String> keyValues = new HashMap<>();
    keyValues.put("data", "");
    this.defaultJsonObject = new JsonObject(keyValues);
  }
  /**
   * 登入Alfresco系統,並取得登入票證(Ticket),支援http與https兩種通訊方式.
   * @return 登入票證
   * @throws IOException if http GET operation fails
   */
  public String getLoginTicket() throws IOException {
    String urlString =
        String
            .format(LOGIN_TICKET_PATTERN, baseUrl, username, password);
    CloseableHttpClient httpClient = createHttpClient();
    HttpGet getRequest = new HttpGet(urlString);
    HttpResponse response = httpClient.execute(getRequest);
    if (response.getStatusLine().getStatusCode() != HttpServletResponse.SC_OK) {
      throw new RuntimeException("Failed : HTTP error code : "
          + response.getStatusLine().getStatusCode());
    }
    String json = IOUtils.toString(response.getEntity().getContent(), "UTF-8");
    JsonObject data = Jsoner.deserialize(json, this.defaultJsonObject);
    JsonKey key = Jsoner.mintJsonKey("data", "");
    Map<String, String> map = data.getMap(key);
    if (map != null && map.containsKey("ticket")) {
      return map.get("ticket");
    }
    return "";
  }


  /**
   * Create http client according to protocol.
   * @return a suitable http client instance
   */
  private CloseableHttpClient createHttpClient() {
    if (this.baseUrl.startsWith("https")) {
      SSLContext sslContext = null;
      try {
        sslContext = new SSLContextBuilder()
            .loadTrustMaterial(null, new TrustStrategy() {
              @Override
              public boolean isTrusted(X509Certificate[] certificate, String authType)
                  throws CertificateException {
                return true;
              }
            }).build();
      } catch (KeyManagementException | NoSuchAlgorithmException | KeyStoreException e) {
        logger.error("create http client with ssl fails", e);
        throw new RuntimeException("create http client with ssl fails");
      }

      return HttpClients.custom().setSSLContext(sslContext)
          .setSSLHostnameVerifier(new NoopHostnameVerifier()).build();
    } else {
      return HttpClientBuilder.create().build();
    }
  }

  /**
   * 登入Alfresco系統,並取得登入會話期(Session),還未曾在https通訊方式上測試過.
   * @return 登入會話期
   */
  public Session getLoginSession() {
    String url = this.baseUrl + LOGIN_SESSION_URI;

    Map<String, String> sessionParameters = new HashMap<String, String>();
    sessionParameters.put(SessionParameter.USER, username);
    sessionParameters.put(SessionParameter.PASSWORD, password);
    sessionParameters.put(SessionParameter.ATOMPUB_URL, url);
    sessionParameters.put(SessionParameter.BINDING_TYPE, BindingType.ATOMPUB.value());
    SessionFactory sessionFactory = SessionFactoryImpl.newInstance();
    return sessionFactory.getRepositories(sessionParameters).get(0).createSession();
  }
}
