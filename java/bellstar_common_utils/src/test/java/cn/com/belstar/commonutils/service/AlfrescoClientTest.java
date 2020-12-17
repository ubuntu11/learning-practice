package cn.com.belstar.commonutils.service;

import static org.junit.Assert.*;
import static org.mockserver.model.HttpRequest.request;
import static org.mockserver.model.HttpResponse.response;
import javax.servlet.http.HttpServletResponse;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.mockserver.client.server.MockServerClient;
import org.mockserver.integration.ClientAndServer;
/**
 * 針對AlfrescoClient進行單元測試的Test Class.
 * @author ken
 *
 */
public class AlfrescoClientTest {
  /**
   * Mock Server所採用的port number.
   */
  private static final int MOCK_SERVER_PORT = 1080;
  /**
   * 假設的Alfresco Login Ticket回應值.
   */
  private static final String TICKET = "TICKET_89fb82c1abae36b832c322070a143742a7a66f96";
  /**
   * 受測對象.
   */
  private AlfrescoClient client = null;
  /**
   * Mock server instance.
   */
  private static ClientAndServer mockServer;
  /**
   * Test Class建立時的初始動作.
   */
  @BeforeClass
  public static void startServer() {
    mockServer = ClientAndServer.startClientAndServer(MOCK_SERVER_PORT);
    setUpTicketResponse();
  }

  /**
   * Test Class完成工作時的清理動作.
   */
  @AfterClass
  public static void stopServer() {
    mockServer.stop();
  }

  /**
   * 設定Mock server應提供的回應值.
   */
  @SuppressWarnings("resource")
  private static void setUpTicketResponse() {
    new MockServerClient("localhost", MOCK_SERVER_PORT)
            .when(request()
                    .withMethod("GET")
                    .withPath("/alfresco/s/api/login")
                    .withQueryStringParameter("u")
                    .withQueryStringParameter("pw")
                    .withQueryStringParameter("format", "json"))
            .respond(response().withStatusCode(HttpServletResponse.SC_OK)
                    .withHeader("Content-Type", "application/json;charset=UTF-8")
            .withBody("{\"data\":{\"ticket\":\"" + TICKET + "\"}}"));
  }

  /**
   * 測試以Http協定,取得Login Ticket.
   * @throws Exception if anything goes wrong
   */
  @Test
  public void testGetLoginTicketByHttp() throws Exception {
    client =
        new AlfrescoClient("http://localhost:1080", "username", "password");
    String ticket = client.getLoginTicket();
    assertEquals("should get correct login ticket", TICKET, ticket);
  }

  /**
   * 測試以Https協定,取得Login Ticket.
   * @throws Exception if anything goes wrong
   */
  @Test
  public void testGetLoginTicketByHttsp() throws Exception {
    client =
        new AlfrescoClient("https://localhost:1080", "username", "password");
    String ticket = client.getLoginTicket();
    assertEquals("should get correct login ticket", TICKET, ticket);
  }

}
