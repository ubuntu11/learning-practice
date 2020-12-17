/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.data.dao;

import java.util.List;
import tw.idv.kenhu.jsfsample.data.entity.GuestMessage;

/**
 * 與使用者相關DB操作的服務介面.
 * @author ken
 */
public interface UserDaoIF extends Dao<Long, GuestMessage> {
    /**
     * 載入所有使用者留言,並根據id做降冪排序.
     * @return a list of User instances ordered ascendingly by id.
     */
    List<GuestMessage> loadAllMessages();
}
