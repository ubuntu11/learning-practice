package cn.com.belstar.commonutils.utils.odoo;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import lombok.Data;
import lombok.experimental.Builder;

/**
 * 查詢odoo時,對符合查詢的結果做篩選所使用的篩選條件. 可以運用builder pattern快速建立出實例：
 * <p>
 * Filter filter =
 * Filter.builder().fieldNames(Arrays.asList("name","email","age")).offset(0).limit(10).build();
 * </p>
 * @author ken
 */
@Data
@Builder
public class Filter {
  /**
   * 代表欄位名稱的字串常數.
   */
  public static final String FILTER_KEY_NAME_FIELDS = "fields";
  /**
   * 代表偏移筆數的字串常數.
   */
  public static final String FILTER_KEY_NAME_OFFSET = "offset";
  /**
   * 代表回傳筆數的字串常數.
   */
  public static final String FILTER_KEY_NAME_LIMIT = "limit";
  /**
   * 回傳的查詢結果應攜帶欄位的欄位名稱,不設定則撈取全部欄位.
   */
  private List<String> fieldNames;
  /**
   * 從所有查詢結果中,只回傳由此值指定的第幾筆開始的查詢結果,不設定則從0開始.
   */
  private Integer offset;
  /**
   * 從所有查詢結果中,只回傳此值所代表的多少筆數的查詢結果,不設定則回傳全部資料.
   */
  private Integer limit;

  /**
   * 轉成透過xmlrpc傳送呼叫時所需要的Map格式,null值會被濾除.
   * @return 呼叫時所需要的Map
   */
  public Map<String, Object> toMap() {
    Map<String, Object> odooFilter = new HashMap<>();

    odooFilter.put(FILTER_KEY_NAME_FIELDS, this.fieldNames);
    odooFilter.put(FILTER_KEY_NAME_OFFSET, this.offset);
    odooFilter.put(FILTER_KEY_NAME_LIMIT, this.limit);
    odooFilter.values().removeAll(Collections.singleton(null));

    return odooFilter;
  }
}
