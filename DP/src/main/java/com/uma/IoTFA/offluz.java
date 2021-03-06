package com.uma.IoTFA;

import java.io.IOException;
import java.security.GeneralSecurityException;

import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.api.client.googleapis.auth.oauth2.GoogleCredential;
import com.google.api.client.googleapis.javanet.GoogleNetHttpTransport;
import com.google.api.client.http.HttpRequestInitializer;
import com.google.api.client.json.JsonFactory;
import com.google.api.client.json.jackson2.JacksonFactory;
import com.google.api.services.cloudiot.v1.CloudIot;
import com.google.api.services.cloudiot.v1.CloudIotScopes;
import com.google.api.services.cloudiot.v1.model.DeviceConfig;
import com.google.api.services.cloudiot.v1.model.ModifyCloudToDeviceConfigRequest;

import java.util.Base64;
import java.util.Date;


@WebServlet(
    name = "offluzAppEngine",
    urlPatterns = {"/offluz"}
)
public class offluz extends HttpServlet {

	
	  public static void setDeviceConfiguration(
		      String deviceId, String projectId, String cloudRegion, String registryName,
		      String data, long version)
		      throws GeneralSecurityException, IOException {
		    GoogleCredential credential =
		        GoogleCredential.getApplicationDefault().createScoped(CloudIotScopes.all());
		    JsonFactory jsonFactory = JacksonFactory.getDefaultInstance();
		    HttpRequestInitializer init = new RetryHttpInitializerWrapper(credential);
		    final CloudIot service = new CloudIot.Builder(
		        GoogleNetHttpTransport.newTrustedTransport(),jsonFactory, init)
		        .setApplicationName("APP_NAMEcambiadopormi").build();

		    final String devicePath = String.format("projects/%s/locations/%s/registries/%s/devices/%s",
		        projectId, cloudRegion, registryName, deviceId);

		    ModifyCloudToDeviceConfigRequest req = new ModifyCloudToDeviceConfigRequest();
		    req.setVersionToUpdate(version);

		    // Data sent through the wire has to be base64 encoded.
		    Base64.Encoder encoder = Base64.getEncoder();
		    String encPayload = encoder.encodeToString(data.getBytes("UTF-8"));
		    req.setBinaryData(encPayload);

		    DeviceConfig config =
		        service
		            .projects()
		            .locations()
		            .registries()
		            .devices()
		            .modifyCloudToDeviceConfig(devicePath, req).execute();

		    System.out.println("Updated: " + config.getVersion());
		  }

	
  @Override
  public void doGet(HttpServletRequest request, HttpServletResponse response) 
      throws IOException {
      
    response.setContentType("text/plain");
    response.setCharacterEncoding("UTF-8");
	
		  try {
			setDeviceConfiguration("domotic-dispositivo","domotic-room",
					  "europe-west1","domotic-registro","offluz",0);
		} catch (GeneralSecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
    
    response.getWriter().print("Hello App Engine!\r\n");

  }
}