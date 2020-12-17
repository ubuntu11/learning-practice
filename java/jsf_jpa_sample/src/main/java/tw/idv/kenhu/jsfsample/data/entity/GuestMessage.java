/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.data.entity;

import java.io.Serializable;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;
import lombok.AccessLevel;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * JPA Entity for 使用者.
 *
 * @author ken
 */
@Entity
@Table(name = "guest_message")
@Data
@Builder
@AllArgsConstructor(access = AccessLevel.PACKAGE)
@NoArgsConstructor(access = AccessLevel.PACKAGE)
public class GuestMessage implements Serializable {
    /**
     * unique id for serialization.
     */
    private static final long serialVersionUID = 1L;
    /**
     * log id for JPA entity主鍵.
     */
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;
    /**
     * 留言者姓名.
     */
    private String name;
    /**
     * 留言內容.
     */
    private String message;

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (id != null ? id.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        if (!(object instanceof GuestMessage)) {
            return false;
        }
        GuestMessage other = (GuestMessage) object;
        return ((this.id == null && other.id != null) || (!this.id.equals(other.id)
                && this.id != null));
    }
}
