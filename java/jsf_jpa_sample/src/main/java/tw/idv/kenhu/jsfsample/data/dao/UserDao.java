/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.data.dao;

import java.util.List;
import org.apache.log4j.Logger;
import javax.faces.bean.ApplicationScoped;
import javax.faces.bean.ManagedBean;
import javax.persistence.TypedQuery;
import tw.idv.kenhu.jsfsample.data.entity.GuestMessage;

/**
 * 與使用者相關DB操作的實作類別.
 * @author ken
 */
@ManagedBean(name = "userDao")
@ApplicationScoped
public class UserDao extends JpaDao<Long, GuestMessage> implements UserDaoIF {
    /**
     * Log4j logger instance.
     */
    private final Logger logger = Logger.getLogger(UserDao.class);

    @Override
    public List<GuestMessage> loadAllMessages() {
        TypedQuery<GuestMessage> query;
        query = entityManager.createQuery("SELECT m FROM GuestMessage m ORDER BY m.id DESC",
                GuestMessage.class);
        List<GuestMessage> result =  query.getResultList();
        logger.debug(String.format("load all messages with %s records", result.size()));
        entityManager.clear();
        return result;
    }

}
