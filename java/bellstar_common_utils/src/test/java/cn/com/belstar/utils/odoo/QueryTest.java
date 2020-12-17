package cn.com.belstar.utils.odoo;

import static org.junit.Assert.*;
import java.util.List;
import org.junit.Test;
import cn.com.belstar.commonutils.utils.odoo.Query;

/**
 * 針對Query class的測試類別.
 * @author ken
 */
public class QueryTest {
  /**
   * 受測Query實例1.
   */
  private Query q1 = Query.builder().fieldName("name").operator(Query.Operator.EQUALS)
      .value("ken").build();
  /**
   * 受測Query實例1.
   */
  private Query q2 = Query.builder().fieldName("age").operator(Query.Operator.GREATER_THAN)
      .value("20").build();
  /**
   * 受測Query實例1.
   */
  private Query q3 = Query.builder().fieldName("age").operator(Query.Operator.LESS_THAN)
      .value("30").build();

  /**
   * Test Query instances building.
   */
  @Test
  public void testQueryBuilding() {
    assertEquals("equals operator should be '='", "=", q1.getOperator().getOperator());
    assertEquals("test value should be 'ken'", "ken", q1.getValue());
    assertEquals("greater_than operator should be '>'", ">", q2.getOperator().getOperator());
    assertEquals("test value should be '20'", "20", q2.getValue());
    assertEquals("less_than operator should be '<'", "<", q3.getOperator().getOperator());
    assertEquals("test value should be '30'", "30", q3.getValue());
  }

  /**
   * Test Query.toList() method.
   */
  @Test
  public void testQueryToList() {
    List<Object> qList = q1.toList();
    assertNotNull("toList() should not return null", qList);
    assertEquals("toList() should contains 3 elements", 3, qList.size());
  }

  @Test
  public void testQueryWithNullValueToList() {
    List<Object> qList = Query.builder().build().toList();
    assertNotNull("toList() should not return null", qList);
    assertEquals("toList() should be empty", 0, qList.size());
  }

}
