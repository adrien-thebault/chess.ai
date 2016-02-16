##
#
#  @author Adrien Thébault <me@adrien-thebault.fr>
#  @brief Artificial intelligence for http://chess.ruby.labs.adrien-thebault.fr/hello.rb/
#

if ARGF.argv.length != 3
  puts 'Usage: artificial-intelligence.rb [game-id] [white|black] [minmax|alphabeta]'
else

  ##
  #  Dependencies

  require 'json'
  require 'net/http'

  require './chess/player'
  require './chess/game/game'

  ## --

  was_my_round = true

  loop do

    ##
    #  Load game data

    url = URI.parse('http://chess.ruby.labs.adrien-thebault.fr/hello.rb/api/get/' + ARGF.argv[0])
    req = Net::HTTP::Get.new(url.to_s)

    res = Net::HTTP.start(url.host, url.port) do |http|
      http.request(req)
    end

    game = Chess::Game.new
    game.from_hash JSON.parse(res.body)

    print "\n\n" << '[INFO]' << "\t" << 'Game loaded' if was_my_round

    ##
    #  Stop if checkmate

    if game.checkmate?

      print "\n" << '[INFO]' << "\t" << 'Checkmate!'

      if ((game.round_player == Chess::Game::PLAYER_WHITE) ? 'white' : 'black') == ARGF.argv[1]
        puts ' I lost, I\'m so sorry..'
      else
        puts ' I won, ahahah, I\'m the best!'
      end

      break

    end

    ##
    #  Play if it's my round

    if ((game.round_player == Chess::Game::PLAYER_WHITE) ? 'white' : 'black') == ARGF.argv[1]

      print "\n\n" << '[INFO]' << "\t" << 'My round!'

      move = Chess::Player.instance.play game, ((ARGF.argv[1] == 'white') ? Chess::Player::PLAYER_WHITE : Chess::Player::PLAYER_BLACK), ((ARGF.argv[2] == 'minmax') ? Chess::Player::MINMAX : Chess::Player::ALPHABETA)

      unless move.nil?

        url = URI.parse('http://chess.ruby.labs.adrien-thebault.fr/hello.rb/api/move/' + ARGF.argv[0] + '/' + move[0] + '/' + move[1])
        req = Net::HTTP::Get.new(url.to_s)

        res = Net::HTTP.start(url.host, url.port) do |http|
          http.request(req)
        end

        print "\n" << '[MOVE]' << "\t" << (move[0] + ' --> ' + move[1])

      end

      was_my_round = true

    else

      print "\n" << '[INFO]' << "\t" << 'Not my round!' if was_my_round
      was_my_round = false

    end

    sleep 1 # to avoid burning the server

  end

end
