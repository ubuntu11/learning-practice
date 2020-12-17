package cn.com.belstar.commonutils.service;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import org.apache.xmlrpc.XmlRpcException;
import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.XmlRpcClientConfigImpl;
import cn.com.belstar.commonutils.utils.odoo.Filter;
import cn.com.belstar.commonutils.utils.odoo.Query;

/**
 * 對Odoo ERP系統進行XmlRpc呼叫的類別.
 * 使用方式舉例如下:
 * <pre>
 * OdooClient client =
        new OdooClient(ODOO_URL, ODOO_DATABASE_NAME, ODOO_USERNAME, ODOO_PASSWORD);

    //不指定查詢條件
    Object[] records1 =
        client.searchRead("product.line", Arrays.asList(Query.builder().build()), Filter.builder()
            .fieldNames(Arrays.asList("id", "name")).build());
    System.out.println(String.format("Find %s records", records1.length));

    //指定查詢條件
    Object[] records2 =
        client.searchRead(
            "sale.order",
            Arrays.asList(Query.builder().fieldName("amount_total")
                .operator(Query.Operator.NOT_EQUAL_TO).value(Integer.valueOf(0)).build()), Filter
                .builder().fieldNames(Arrays.asList("id", "name")).build());
    System.out.println(String.format("Find %s records", records2.length));
 * </pre>
 * @author ken
 *
 */
public class OdooClient {
  /**
   * 透過這個文字樣式來產生查詢資料用的完整URL.
   */
  private static final String URL_PATTERN_FOR_QUERY = "%s/xmlrpc/2/object";
  /**
   * 透過這個文字樣式來產生登入odoo用的完整URL.
   */
  private static final String URL_PATTERN_FOR_AUTHENTICATION = "%s/xmlrpc/2/common";
  /**
   * odoo url.
   */
  private String url;
  /**
   * 登入odoo所使用的user name.
   */
  private String username;
  /**
   * 登入odoo所使用的password.
   */
  private String password;
  /**
   * odoo所使用的資料庫.
   */
  private String database;

  /**
   * odoo使用者Id.
   */
  private Integer uid;

  /**
   * Default constructor.
   * @param odooUrl odoo erp url
   * @param odooDatabase odoo所使用的資料庫
   * @param odooUsername 登入odoo所使用的user name
   * @param odooPasword 登入odoo所使用的password
   */
  public OdooClient(String odooUrl, String odooDatabase, String odooUsername, String odooPasword) {
    this.url = odooUrl;
    this.database = odooDatabase;
    this.username = odooUsername;
    this.password = odooPasword;
  }

  /**
   * 登入odoo並取得odoo使用者Id.
   * @return uid odoo使用者Id
   * @throws MalformedURLException if 提供了格式錯誤的url
   * @throws XmlRpcException if xmlRpcClient執行遠方procedure失敗
   */
  private Integer authenticate() throws MalformedURLException, XmlRpcException {
    return (Integer) getXmlRpcClient(URL_PATTERN_FOR_AUTHENTICATION).execute("authenticate",
        Arrays.asList(database, username, password, Collections.EMPTY_LIST));
  }

  /**
   * 執行xmlrpc searchRead 方法.
   * @param module 指定查詢的module
   * @param queries 指定查詢條件,此參數為空時,等同不設查詢條件
   * @param filter 對符合查詢條件的結果,指定篩選方式,例如:欄位名稱,開始筆數,回傳筆數.欄位名稱盡量要設定,因為全部撈取會花
   * 非常多時間.
   * @return Object[] 符合查詢條件及篩選方式的結果
   * @throws MalformedURLException if 提供了格式錯誤的url
   * @throws XmlRpcException if xmlRpcClient執行遠方procedure失敗
   * @since 2018/1/16
   */
  public Object[] searchRead(String module, List<Query> queries, Filter filter)
      throws MalformedURLException, XmlRpcException {

    Object[] records =
        (Object[]) getXmlRpcClient(URL_PATTERN_FOR_QUERY).execute(
            "execute_kw",
            new Object[] {database, getUid(), password, module, "search_read",
                convertOdooQueries(queries), filter.toMap()});

    return records;
  }

  /**
   * 對odoo進行查詢前,先轉換格式為xmlrpc所要傳遞的格式.
   * @param queries 對odoo進行的查詢條件.
   * @return 透過xmlrpc傳遞的odoo查詢條件//List<List<Object>
   */
  private List<Object> convertOdooQueries(List<Query> queries) {
    List<Object> firstTierList = new LinkedList<>();
    if (queries == null || queries.isEmpty()) {
      return firstTierList;
    }

    List<Object> secondTierList = new LinkedList<>();
    for (Query q : queries) {
      if (!q.toList().isEmpty()) {
        secondTierList.add(q.toList());
      }
    }

    if (!secondTierList.isEmpty()) {
      firstTierList.add(secondTierList);
    }

    return firstTierList;
  }

  /**
   * 執行xmlrpc read 方法.
   * @param moudle 指定撈取的module
   * @param id 指定撈取的id<br>
   * @param filter 篩選條件,用來指定撈取的欄位名稱,盡量要設定,全部撈取會花非常多時間
   * @return Object
   * @throws MalformedURLException if 提供了格式錯誤的url
   * @throws XmlRpcException if xmlRpcClient執行遠方procedure失敗
   * @since 2018/1/16
   */
  public Object read(String moudle, int id, Filter filter)
      throws MalformedURLException, XmlRpcException {

    Object records =
        getXmlRpcClient(URL_PATTERN_FOR_QUERY).execute(
            "execute_kw",
            new Object[] {database, getUid(), password, moudle, "read", Arrays.asList(id),
                filter.toMap()});

    return records;
  }

  /**
   * Use this getter method to delay authentication call, its main purpose is for unit testing.
   * @return odoo user id
   * @throws MalformedURLException if url is malfomred
   * @throws XmlRpcException if xml rpc call runs into problem
   */
  private Integer getUid() throws MalformedURLException, XmlRpcException {
    if (uid == null) {
      uid = authenticate();
    }
    return uid;
  }

  /**
   * Use this getter method to delay the initiation, its main purpose is for unit testing.
   * @param urlPattern pattern to convert url to real url string
   * @return XmlRpcClient instance
   * @throws MalformedURLException if url is malfomred
   */
  protected XmlRpcClient getXmlRpcClient(String urlPattern) throws MalformedURLException {
    XmlRpcClient xmlRpcClient = new XmlRpcClient();
    XmlRpcClientConfigImpl xmlRpcClientConfigImpl = new XmlRpcClientConfigImpl();
    xmlRpcClientConfigImpl.setServerURL(new URL(String.format(urlPattern, url)));
    xmlRpcClient = new XmlRpcClient();
    xmlRpcClient.setConfig(xmlRpcClientConfigImpl);

    return xmlRpcClient;
  }

  /**
   * Sample usages listed here.
   * @param args console arguments
   * @throws Exception if anything goes wrong
   */
  public static void main(String[] args) throws Exception {
    OdooClient client =
        new OdooClient("https://odoo.bizcloud.xyz", "Mancy0803", "develop@belstar.com.cn",
            "belstar1234");
    Object[] records1 =
        client.searchRead("product.line", Arrays.asList(Query.builder().build()), Filter.builder()
            .fieldNames(Arrays.asList("id", "name")).build());
    System.out.println(String.format("Find %s records", records1.length));

    Object[] records2 =
        client.searchRead(
            "sale.order",
            Arrays.asList(Query.builder().fieldName("amount_total")
                .operator(Query.Operator.NOT_EQUAL_TO).value(Integer.valueOf(0)).build()), Filter
                .builder().fieldNames(Arrays.asList("id", "name")).build());
    System.out.println(String.format("Find %s records", records2.length));
  }

}
