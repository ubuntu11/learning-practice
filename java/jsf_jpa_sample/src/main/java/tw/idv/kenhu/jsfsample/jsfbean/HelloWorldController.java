/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
package tw.idv.kenhu.jsfsample.jsfbean;

import java.util.List;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.ManagedProperty;
import javax.faces.bean.RequestScoped;
import javax.faces.context.FacesContext;
import tw.idv.kenhu.jsfsample.data.entity.GuestMessage;
import tw.idv.kenhu.jsfsample.service.SampleServiceIF;

/**
 * A typical simple backing bean, that is backed to <code>helloWorld.xhtml</code>.
 */
@ManagedBean(name = "helloWorld")
@RequestScoped
public class HelloWorldController {

    /**
     * Guest name.
     */
    private String name;
    /**
     * Guest's message.
     */
    private String message;
    /**
     * Existed user messages.
     */
    private List<GuestMessage> messages;

    /**
     * @return the message
     */
    public String getMessage() {
        return message;
    }

    /**
     * @param message the message to set
     */
    public void setMessage(String message) {
        this.message = message;
    }

    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * @return the registeredUsernames
     */
    public List<GuestMessage> getMessages() {
        return service.loadAllMessages();
    }

    /**
     * @return the service
     */
    public SampleServiceIF getService() {
        return service;
    }

    /**
     * @param service the service to set
     */
    public void setService(SampleServiceIF service) {
        this.service = service;
    }
    /**
     * Service class instance.
     */
    @ManagedProperty(value = "#{sampleService}")
    private SampleServiceIF service;

    /**
     * default empty constructor.
     */
    public HelloWorldController() {

    }

    /**
     * Method that is backed to a submit button of a form.
     */
    public void send() {
        GuestMessage guest = GuestMessage.builder().name(name).message(message).build();
        refreshPage();
        service.sendMessage(guest);
    }

    public String getName() {
        return name;
    }
    /**
     * 更新bean properties,並反映到頁面上.
     */
    private void refreshPage() {
        this.name = "";
        this.message = "";
        FacesContext.getCurrentInstance().getPartialViewContext().getRenderIds().add("messages");
    }

}
