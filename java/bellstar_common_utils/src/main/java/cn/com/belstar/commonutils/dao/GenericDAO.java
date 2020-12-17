package cn.com.belstar.commonutils.dao;

import java.io.Serializable;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import org.hibernate.Criteria;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.criterion.Projections;
import org.hibernate.criterion.Restrictions;

/**
 * generic dao. 簡化操作hibernate所需要的程式碼 同時限制可使用的功能
 * @author Belstar
 *
 * @param <T>
 */
public class GenericDAO<T> {
  /**
   * 指定查詢所需的表對應的hibernate entity.
   */
  private final Class<T> typeParameterClass;
  /**
   * create by SessionFactory.
   */
  private Session session;

  /**
   * constructor. need a session to allow use transation.
   * @param typeParameterClass hibernate entity
   * @param session use HibernateUtil.getSessionFactory() to create session.
   */
  public GenericDAO(Class<T> typeParameterClass, Session session) {
    this.typeParameterClass = typeParameterClass;
    this.session = session;
  }

  /**
   * get the size of result.
   * @return the size of result.
   */
  public Long count() {
    Criteria criteria = session.createCriteria(typeParameterClass);
    criteria.setProjection(Projections.rowCount());
    Long count = (Long) criteria.uniqueResult();
    return count;
  }

  /**
   * get all data from table.
   * @return list of entity class
   */
  public List<T> find() {
    Criteria criteria = session.createCriteria(typeParameterClass);
    @SuppressWarnings("unchecked")
    List<T> entityList = criteria.list();
    return entityList;
  }

  /**
   * get data in the table match to entity class use start and limit to set range.
   * @param start first result
   * @param limit max result
   * @return list of entity class
   */
  public List<T> find(int start, int limit) {
    Criteria criteria = session.createCriteria(typeParameterClass);
    criteria.setFirstResult(start);
    criteria.setMaxResults(limit);
    @SuppressWarnings("unchecked")
    List<T> entityList = criteria.list();
    return entityList;
  }

  /**
   * Find by id.
   * @param id pk
   * @return entityClass
   */
  public T findById(long id) {
    @SuppressWarnings("unchecked")
    T entity = (T) session.get(typeParameterClass, id);
    return entity;
  }

  /**
   * Find by field.
   * @param field table's column
   * @param value value
   * @return list of entity class
   */
  public List<T> findByField(String field, Object value) {
    Criteria criteria = session.createCriteria(typeParameterClass);
    criteria.add(Restrictions.eq(field, value));
    @SuppressWarnings("unchecked")
    List<T> resultList = criteria.list();
    return resultList;
  }

  /**
   * Find by field and limit result.
   * @param entityClass 你希望取得資料的table所對應的entityClass
   * @param field table's column
   * @param value value
   * @param start first result
   * @param limited max result
   * @return list of entity class
   */
  public List<T> findByField(Class<T> entityClass, String field, Object value, int start,
      int limited) {
    Criteria criteria = session.createCriteria(entityClass);
    criteria.add(Restrictions.eq(field, value));
    criteria.setFirstResult(start);
    criteria.setMaxResults(limited);
    @SuppressWarnings("unchecked")
    List<T> resultList = criteria.list();
    return resultList;
  }

  /**
   * save.
   * @param entity 單一一筆資料
   * @return Serializable
   */
  public Serializable save(T entity) {
    Serializable serializable = session.save(entity);
    return serializable;
  }

  /**
   * Update.
   * @param entity 單一一筆資料 須包含id
   */
  public void update(T entity) {
    session.update(entity);
  }

  /**
   * Delete.
   * @param entity 單一一筆資料 須包含id
   */
  public void delete(Object entity) {
    session.delete(entity);
  }

  /**
   * Query.
   * @param queryString sql script
   * @return list of entity class
   */
  public List<T> query(String queryString) {
    Query query = session.createQuery(queryString);
    @SuppressWarnings("unchecked")
    List<T> resultList = query.list();
    return resultList;
  }

  /**
   * Query by parameter.
   * @param queryString sql script
   * @param parameterMap parameter
   * @return list of entity class
   */
  public List<T> queryByParam(String queryString, Map<String, Object> parameterMap) {
    Query query = session.createQuery(queryString);
    for (Entry<String, Object> entry : parameterMap.entrySet()) {
      query.setParameter(entry.getKey(), entry.getValue());
    }
    @SuppressWarnings("unchecked")
    List<T> resultList = query.list();
    return resultList;
  }
}
