#!/usr/bin/env ruby

require "rubygems"
require "bundler/setup"
require "sinatra"
require "active_support"
require "pathname"
require "tilt"

configure(:development) do |c|
  require "sinatra/reloader"
  c.also_reload "listen.rb"
end

require "./listen.rb"

run Sinatra::Application
