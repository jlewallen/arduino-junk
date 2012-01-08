#!/usr/bin/env ruby

require 'twitter'
require 'yaml'

def symbolize_keys(hash)  
  hash.inject({}) { |r, (key, value)|
    new_key = case key  
      when String then key.to_sym  
      else key  
    end  
    new_value = case value  
      when Hash then symbolize_keys(value)  
      else value  
    end  
    r[new_key] = new_value  
    r
  }  
end

configuration = symbolize_keys(YAML.load_file("configuration.yml"))

Twitter.configure do |config|
  config.consumer_key = configuration[:twitter][:consumer_key]
  config.consumer_secret = configuration[:twitter][:consumer_secret]
  config.oauth_token = configuration[:twitter][:oauth_token]
  config.oauth_token_secret = configuration[:twitter][:oauth_token_secret]
end

post '/tweet' do
  Twitter.update(params[:status])
  "Ok"
end

get '/' do
  "Ok"
end

# EOF
