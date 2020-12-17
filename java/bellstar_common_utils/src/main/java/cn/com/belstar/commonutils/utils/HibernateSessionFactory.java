package cn.com.belstar.commonutils.utils;

import java.util.Properties;
import org.hibernate.SessionFactory;
import org.hibernate.cfg.Configuration;
import org.hibernate.service.ServiceRegistry;
import org.hibernate.service.ServiceRegistryBuilder;

/**
 * 根據傳入的系統屬性值,建立Hibernate Session Factory,供其它程式取用.
 * @author totem
 *
 */
public class HibernateSessionFactory {
  /**
   * 系統屬性值.
   */
  private Properties properties;

  /**
   * 預設constructor.
   * @param props 系統屬性值
   */
  public HibernateSessionFactory(Properties props) {
    properties = props;
    init();
  }

  /**
   * Hibernate SessionFactory.
   */
  private SessionFactory sessionFactory;

  /**
   * User properties to initialize hibernate session factory.
   */
  private void init() {
    try {
      ServiceRegistryBuilder serviceRegistryBuilder = new ServiceRegistryBuilder();
      serviceRegistryBuilder.applySettings(properties);
      ServiceRegistry serviceRegistry = serviceRegistryBuilder.buildServiceRegistry();

      Configuration configuration = new Configuration().configure();
      configuration.setProperties(properties);
      sessionFactory = configuration.buildSessionFactory(serviceRegistry);
    } catch (Throwable e) {
      throw new ExceptionInInitializerError(e);
    }
  }

  public SessionFactory getSessionFactory() {
    return sessionFactory;
  }
}
