begin
  require 'openssl'
rescue LoadError
  exit 1
end

key = OpenSSL::PKey::RSA.new 2048
