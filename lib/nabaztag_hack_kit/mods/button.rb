module NabaztagHackKit
  module Mods
    module Button

      def self.registered(app)
        app.on "button-pressed" do |bunny, data, request, run|
          callback("button-pressed", bunny, params[:duration], request, run+1)
        end
      end

    end
  end
end
