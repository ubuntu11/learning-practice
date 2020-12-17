/*
 * Term.java    1.0 2017年5月23日
 *
 * Copyright (c) 2017-2030 Monmouth Technologies, Inc.
 * http://www.mt.com.tw
 * 10F-1 No. 306 Chung-Cheng 1st Road, Linya District, 802, Kaoshiung, Taiwan
 * All Rights Reserved.
 *
 * This software is the confidential and proprietary information of Monmouth
 * Technologies, Inc. You shall not disclose such Confidential Information and
 * shall use it only in accordance with the terms of the license agreement you
 * entered into with Monmouth Technologies.
 */
package tw.com.mt.ckipparser;

/**
 * 儲存ckip parser分析結果裡的每一個斷詞的類別.以#1:1.[0] S(NP(N:手機|Head:Nv:上網)|ADV:如何|
 * Head:Vt:設定)#?(QUESTIONCATEGORY)這樣的結果為例,會得到6個Term.
 *
 * @version 1.0 2017年5月23日
 * @author ken
 *
 */
public class Term {
    /**
     * Text part of this term,ex: 上網.
     */
    private String text = "";
    /**
     * Pos(part of speech, 詞類) of this term,ex: Nv.
     */
    private String pos = "";
    /**
     * The thematic role(語意角色) of this term,ex: Head.
     */
    private String thematicRole = "";
    /**
     * Term generated after segmentation with pos and thematic role.
     */

    /**
     * @return the text
     */
    public final String getText() {
        return text;
    }
    /**
     * @param text the text to set
     */
    public final void setText(String text) {
        this.text = text;
    }
    /**
     * @return the pos
     */
    public final String getPos() {
        return pos;
    }
    /**
     * @param pos the pos to set
     */
    public final void setPos(String pos) {
        this.pos = pos;
    }
    /**
     * @return the thematicRole
     */
    public final String getThematicRole() {
        return thematicRole;
    }
    /**
     * @param thematicRole the thematicRole to set
     */
    public final void setThematicRole(String thematicRole) {
        this.thematicRole = thematicRole;
    }

    @Override
    public String toString() {
        return String.format("[role , pos , text ] = [%s , %s , %s]", this.thematicRole, this.pos,
                this.text);
    }
}
