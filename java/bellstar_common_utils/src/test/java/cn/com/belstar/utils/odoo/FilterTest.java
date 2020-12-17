package cn.com.belstar.utils.odoo;

import static org.junit.Assert.*;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import org.junit.Before;
import org.junit.Test;
import cn.com.belstar.commonutils.utils.odoo.Filter;

/**
 * 針對Filter class的測試類別.
 * @author ken
 */
public class FilterTest {
  /**
   * 受測對象.
   */
  private Filter filter;

  /**
   * 每個測試案例執行前的設定動作.
   */
  @Before
  public void setUp() {
    filter =
        Filter.builder().fieldNames(Arrays.asList("name", "email", "age")).offset(1).limit(2)
            .build();
  }

  /**
   * 測試使用builder pattern建立出來的Filter實例.
   */
  @Test
  public void testFilterBuilding() {
    assertNotNull("instance should not bt null", filter);
    assertEquals("there should be 3 fields", 3, filter.getFieldNames().size());
    assertEquals("offset should be 1", 1, filter.getOffset().intValue());
    assertEquals("limit should be 2", 2, filter.getLimit().intValue());
  }

  /**
   * Test Filter.toMap().
   */
  @SuppressWarnings("unchecked")
  @Test
  public void testToMap() {
    Map<String, Object> map = filter.toMap();
    assertNotNull("map should not bt null", map);
    assertEquals("there should be 3 fields", 3,
        ((List<String>) map.get(Filter.FILTER_KEY_NAME_FIELDS)).size());
    assertEquals("offset should be 1", 1,
        ((Integer) map.get(Filter.FILTER_KEY_NAME_OFFSET)).intValue());
    assertEquals("limit should be 2", 2,
        ((Integer) map.get(Filter.FILTER_KEY_NAME_LIMIT)).intValue());
  }

  /**
   * Test converting a Filter instance with null values, null values should be removed.
   */
  @SuppressWarnings("unchecked")
  @Test
  public void testToMapWithNull() {
    filter =
        Filter.builder().fieldNames(Arrays.asList("name", "email", "age")).build();
    Map<String, Object> map = filter.toMap();
    assertNotNull("map should not bt null", map);
    assertEquals("there shoule be only 1 element in map", 1, map.values().size());
    assertEquals("there should be 3 fields", 3,
        ((List<String>) map.get(Filter.FILTER_KEY_NAME_FIELDS)).size());
  }

}
