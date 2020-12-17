package cn.com.belstar.commonutils.utils.odoo;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import lombok.experimental.Builder;
import lombok.Data;

/**
 * 對Odoo進行查詢的條件.
 * @author ken
 */
@Data
@Builder
public class Query {
  /**
   * 要查詢的欄位名稱.
   */
  private String fieldName;
  /**
   * 查詢時所使用的邏輯運算符.
   */
  private Operator operator;
  /**
   * 查詢時進行比對的目標值.
   */
  private Object value;

  /**
   * 用於Odoo查詢的邏輯運算符enum類別.
   * @author ken
   */
  public enum Operator {
    /**
     * 等於.
     */
    EQUALS("="),
    /**
     * 不等於.
     */
    NOT_EQUAL_TO("!="),
    /**
     * 大於.
     */
    GREATER_THAN(">"),
    /**
     * 大於等於.
     */
    GREATER_THAN_OR_EQUAL_TO(">="),
    /**
     * 小於.
     */
    LESS_THAN("<"),

    /**
     * 小於等於.
     */
    LESS_THAN_OR_EQUAL_TO("<="),
    /**
     * 相似, value 需要詳細定義相似範圍. <br>
     * _ （底線） ： 匹配單個字符<br>
     * ％ （百分比）：匹配0個或多個字符<br>
     * <p>matches field_name against the value pattern. An underscore _ in the pattern stands for
     * (matches) any single character; a percent sign % matches any string of zero or more
     * characters.
     */
    EQUAL_LIKE("=like"),
    /**
     * 相似, value 不需要額外寫％係同會在前後自動補上. <br>
     * <p>matches field_name against the %value% pattern. Similar to =like but wraps value with '%'
     * before matching.
     */
    LIKE("like"),
    /**
     * 包含於.
     */
    IN("in");


    /**
     * 邏輯運算符字串值.
     */
    private String operator = "";

    /**
     * Default constructor.
     * @param operator 邏輯運算符字串值
     */
    Operator(String operator) {
      this.operator = operator;
    }

    public String getOperator() {
      return this.operator;
    }
  }

  /**
   * 轉換成透過xmlrpc傳送呼叫時,所需要的List格式.
   * @return 傳送呼叫時所需要的List
   */
  @SuppressWarnings("unchecked")
  public List<Object> toList() {
    if (getFieldName() == null || getOperator() == null || getValue() == null) {
      return Collections.EMPTY_LIST;
    }
    return Arrays.asList(getFieldName(), getOperator().getOperator(), getValue());
  }
}
