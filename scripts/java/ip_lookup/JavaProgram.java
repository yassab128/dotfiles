import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;

public class JavaProgram {
	static void checkIpAddress(NetworkInterface netint, int timeout,
				   String address, InetAddress inetAddress)
	    throws Exception {
		if (inetAddress.isReachable(netint, 1, timeout)) {
			System.out.println(address + ": " +
					   inetAddress.getHostName());
		}
	}
	public static void main(String args[]) throws Exception {
		Enumeration<NetworkInterface> nets;
		nets = NetworkInterface.getNetworkInterfaces();
		NetworkInterface netint = nets.nextElement();
		while (nets.hasMoreElements()) {
			if (!netint.isLoopback() && netint.isUp()) {
				break;
			}
			netint = nets.nextElement();
		}
		System.out.println("Name: " + netint.getName());
		String myIp =
		    netint.getInetAddresses().nextElement().getHostAddress();
		String[] ipAddressPart = myIp.split("\\.");
		String partialNetmask = ipAddressPart[0] + "." +
					ipAddressPart[1] + "." +
					ipAddressPart[2] + ".";
		short myIpLastPart = Short.parseShort(ipAddressPart[3]);
		short i = 1;
		final short timeout = 100;
		System.out.println(myIp + " (You)");
		for (String address; i < myIpLastPart; ++i) {
			address = partialNetmask + i;
			checkIpAddress(netint, timeout, address,
				       InetAddress.getByName(address));
		}
		++i;
		for (String address; i < 255; ++i) {
			address = partialNetmask + i;
			checkIpAddress(netint, timeout, address,
				       InetAddress.getByName(address));
		}
	}
}
