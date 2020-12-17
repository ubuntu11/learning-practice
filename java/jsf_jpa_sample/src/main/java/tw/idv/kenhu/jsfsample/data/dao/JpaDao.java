/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.data.dao;

import java.lang.reflect.ParameterizedType;
import javax.persistence.EntityManager;
import org.apache.log4j.Logger;

/**
 *
 * @author ken
 * @param <K> key
 * @param <E> entity
 */
public abstract class JpaDao<K, E> implements Dao<K, E> {
    /**
     * Log4j logger instance.
     */
    private final Logger logger = Logger.getLogger(this.getClass());
    /**
     * Entity的實際類別.
     */
    protected Class<E> entityClass;
    /**
     * JPA EntityManager instance.
     */
    protected EntityManager entityManager = EntityManagerFactory.getEntityManager();
    /**
     * Default constructor,會利用反射機制找出Entity的實際類別.
     */
    public JpaDao() {
        ParameterizedType genericSuperclass = (ParameterizedType) getClass().getGenericSuperclass();
        this.entityClass = (Class<E>) genericSuperclass.getActualTypeArguments()[1];
    }

    @Override
    public void persist(E entity) {
        logger.info(String.format("persist %s entity to database", entityClass.getName()));
        entityManager.persist(entity);
    }

    @Override
    public void remove(E entity) {
        entityManager.remove(entity);
    }

    @Override
    public E findById(K id) {
        return entityManager.find(entityClass, id);
    }
}
