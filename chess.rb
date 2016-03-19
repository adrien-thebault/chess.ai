#!/usr/bin/env ruby

##
#
#  @author Adrien Thébault <me@adrien-thebault.fr>
#  @brief Artificial intelligence for http://chess.ruby.labs.adrien-thebault.fr/hello.rb/
#

##
#  Dependencies

require 'colorize'
require 'json'
require 'net/http'

require './chess/player/player'
require './chess/game/game'

##
#  Constants

API_HOST,API_PORT = '127.0.0.1', 8080

INFO, ERROR, WARNING, SUCCESS = (Class.new {
  def <<(str); STDOUT << ('[INFO]   ' << "\t" << str << "\n").colorize(:blue) end
}.new),(Class.new {
  def <<(str); STDOUT << ('[ERROR]  ' << "\t" << str << "\n").colorize(:red) end
}.new),(Class.new {
  def <<(str); STDOUT << ('[WARNING]' << "\t" << str << "\n").colorize(:yellow) end
}.new),(Class.new {
  def <<(str); STDOUT << ('[SUCCESS]' << "\t" << str << "\n").colorize(:green) end
}.new)

if ARGF.argv.length != 2 then ERROR << 'Usage: ./chess.rb [game-id] [white|black]'
else

  @game_id, @player, @was_my_round, res = ARGF.argv[0], ((ARGF.argv[1] == 'white') ? Chess::Game::WHITE : Chess::Game::BLACK), true, nil

  loop do

    ##
    #  Load game data

    begin
      Net::HTTP.start(API_HOST, API_PORT) do |http| res = http.request(Net::HTTP::Get.new('/api/get/' + @game_id)) end
    rescue Exception => e
      ERROR << 'HTTP Request failed (' + e.class.name + ')'
      exit
    else
      unless res.code == '200'
        ERROR << 'HTTP Request failed (' + res.code.to_s + ')'
        exit
      end
    end

    @game = JSON.parse(res.body, :symbolize_names => true)
    @game[:round][:end_time] = Time.now.to_i+@game[:round][:duration] if @game[:round][:end_time] == -1

    puts if @was_my_round
    INFO << 'Game loaded' if @was_my_round

    ##
    #  Stop if checkmate

    if Chess::Game.checkmate?(@game, @game[:round][:player])

      INFO << 'Checkmate!'

      if @game[:round][:player] == @player then WARNING << 'I lost, I\'m so sorry..'
      else SUCCESS << 'I won, ahahah, I\'m the best!' end

      break

    elsif Chess::Game.draw?(@game, @game[:round][:player])

      INFO << 'Draw!'
      break

    end

    ##
    #  Play if it's my round

    if @game[:round][:player] == @player

      INFO << 'My round!'

      move = Chess::Player.play(@game, @player)
      unless true || move.nil?

        begin
          Net::HTTP.start(API_HOST, API_PORT) do |http| res = http.request(Net::HTTP::Get.new('/api/move/' + @game_id + '/' + move[0] + '/' + move[1])) end
        rescue Exception => e
          ERROR << 'HTTP Request failed (' + e.class.name + ')'
          exit
        else
          unless res.code == '200'
            ERROR << 'HTTP Request failed (' + res.code.to_s + ')'
            exit
          end
        end

        SUCCESS << (move[0] + ' --> ' + move[1])

      end

      @was_my_round = true

    else

      WARNING << 'Not my round!' if @was_my_round
      @was_my_round = false

    end

    sleep 1

  end

  puts

end
