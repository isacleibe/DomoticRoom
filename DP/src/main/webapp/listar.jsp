<!DOCTYPE html>
<%@ page contentType="text/html;charset=UTF-8" language="java"%>

<%@ page
	import="com.google.appengine.api.datastore.DatastoreServiceFactory"%>
<%@ page import="com.google.appengine.api.datastore.DatastoreService"%>
<%@ page import="com.google.appengine.api.datastore.Query.Filter"%>
<%@ page
	import="com.google.appengine.api.datastore.Query.FilterPredicate"%>
<%@ page
	import="com.google.appengine.api.datastore.Query.FilterOperator"%>
<%@ page
	import="com.google.appengine.api.datastore.Query.CompositeFilter"%>
<%@ page import="com.google.appengine.api.datastore.Query.SortDirection"%>
<%@ page
	import="com.google.appengine.api.datastore.Query.CompositeFilterOperator"%>
<%@ page import="com.google.appengine.api.datastore.Query"%>
<%@ page import="com.google.appengine.api.datastore.PreparedQuery"%>
<%@ page import="com.google.appengine.api.datastore.Entity"%>


<%@ page import="java.util.List"%>
<%@ page import="com.google.appengine.api.users.User"%>
<%@ page import="com.google.appengine.api.users.UserService"%>
<%@ page import="com.google.appengine.api.users.UserServiceFactory"%>

<%@page import="java.util.ArrayList"%>

<%
UserService userService = UserServiceFactory.getUserService();
User user = userService.getCurrentUser();

if (user==null) {	
	out.println("<p>Please <a href=\""
	        + userService.createLoginURL(request.getRequestURI())
	        + "\">sign in</a>.</p>");
	return; // No muy "fino"
	}

//Get the Datastore Service
DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
%>

<html>
<head>
<style type="text/css">
html, body {
	height: 100%;
	margin: 0;
	padding: 0;
}

#map {
	height: 100%;
}
</style>

</head>
<body>

	<%
	//Use class Query to assemble a query
	Query q = new Query("domotic-dato");
				//	.addSort("instante", SortDirection.ASCENDING);
					
	//Use PreparedQuery interface to retrieve results
	PreparedQuery pq = datastore.prepare(q);

	int i=0;
	for (Entity result : pq.asIterable()) {
	    out.println(result.getProperty("tipo")+" "+result.getProperty("valor")+"<br>");
	     i++;
		}
	 %>

</body>
</html>
