package engine.saxhandlers;

import engine.Community;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class TagsHandler extends DefaultHandler {

    private final Community com;

    public TagsHandler(Community com){
        super();
        this.com = com;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException{
        super.startElement(uri, localName, qName, attributes);
        if(!localName.equals("row")) return;
        long id = Long.parseLong(attributes.getValue("Id"));
        String tagName = attributes.getValue("TagName");
        this.com.addTag(id, tagName);
    }
}