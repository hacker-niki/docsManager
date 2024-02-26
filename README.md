# docs Manager

## Project Description

This project is a document management system for a company. It allows users to create, edit, view, and delete documents. The system also includes a document generator that can be used to generate documents from templates.

**Nikita**
Group Number: **253505**

### System Class Diagram

[Image of system class diagram]

### Application Functions

    Create Document: This function allows users to create new documents. Users can specify the document type, name, and content.
    Edit Document: This function allows users to edit existing documents. Users can change the document type, name, and content.
    View Document: This function allows users to view existing documents. Users can see the document type, name, and content.
    Delete Document: This function allows users to delete existing documents.
    Generate Document: This function allows users to generate documents from templates. Users can specify the template name and the data to be used to generate the document.

### Data Models

    Document: The Document class represents a document in the system. 
    It has the following properties:
    - ID
    - Type
    - Name
    - Content
  
    Template: The Template class represents a template that can be used to generate documents. 
      It has the following properties:
    - ID
    - Name
    - Content
    - fields
