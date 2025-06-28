
import os
import subprocess

class ProxyChecker:
    def __init__(self, path_proxy):
        self.path_proxy = path_proxy

    def check(self):
        # Check if the proxy file exists
        if not os.path.exists(self.path_proxy):
            error_msg = f"  [ProxyChecker] [Error] : Proxy file does not exist. Please make the proxy with command [ voms-proxy-init --voms cms --valid 96:00 --out {self.path_proxy} ]"
            print(error_msg)
            raise FileNotFoundError(error_msg)

        # Check the validity time left of the proxy
        try:
            result = subprocess.run(
                ['voms-proxy-info', '-file', self.path_proxy, '--timeleft'],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            if result.returncode != 0:
                error_msg = f"  [ProxyChecker] Checking proxy validity: {result.stderr}"
                print(error_msg)
                raise subprocess.CalledProcessError(
                    result.returncode, result.args, output=result.stdout, stderr=result.stderr
                )

            time_left = int(result.stdout.strip())
            if time_left <= 0:
                error_msg = "  [ProxyChecker] [Error] : Proxy has expired."
                print(error_msg)
                raise RuntimeError(error_msg)
            else:
                print(f"  [ProxyChecker] Proxy is valid for {time_left} seconds.")

        except Exception as e:
            print(f"  [ProxyChecker] [Error] Checking proxy validity: {e}")
            raise

